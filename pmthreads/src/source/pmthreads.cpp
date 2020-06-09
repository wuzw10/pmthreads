/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <cstddef>
#include <new>

#include "locks/posixlock.h"
#include "real.h"
#include "debug.h"
#include "dthread-heaps/dthreadheap.h"
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include "internal_heap.h"
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>


#define unlikely(x)     __builtin_expect((x),0)
volatile bool inTransaction = false;
enum { MAX_LOCAL_BUFFER_SIZE = 256 * 131072 };
static char initBuffer[MAX_LOCAL_BUFFER_SIZE];
static char * initBufferPtr = initBuffer;

static bool __mylib_initialized__ = false;
//thread_local int _heapid = 0;
#include "pmthreads.h"

/* nvm transactions vars */
pthread_mutex_t __mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t __cond = PTHREAD_COND_INITIALIZER;;
unsigned long volatile on_line_tx = 0;
#include "logger.h"
thread_local Logger logger;
#ifndef PARSEC_TEST
thread_local volatile bool in_transaction = false;
#else
thread_local volatile bool in_transaction = false;
#endif

thread_local unsigned long __locks_acquired__ = 0;
thread_local unsigned __missed__ = 0;
#ifndef MAX_MISS
#define MAX_MISS 200
#endif


unsigned COMMIT_INTERVAL = 99999999;
int delaycount = 3500;
Logger* entries[max_threads];

auto __start = std::chrono::steady_clock::now();
thread_local auto __end = std::chrono::steady_clock::now();

thread_local auto duration_tx_begin = std::chrono::steady_clock::now();
thread_local auto duration_tx_end  = std::chrono::steady_clock::now();


#ifdef STAT_BYTES
static auto enter_barrier = std::chrono::steady_clock::now();
static auto exit_barrier = std::chrono::steady_clock::now();
#endif

std::atomic<bool> commit_flag;

size_t __commits__ = 0;
double __avg_commit_time__ = 0;
double __total_commit_time = 0;

transaction_manager* get_transaction_manager()
{
    static char buf[sizeof(transaction_manager)];
    static transaction_manager * zm = new (buf) transaction_manager();
    return zm;
}


static void inline _trigger_alarm(){
    abort();
    fprintf(stderr, "SIGALRM handling deprecated. Check the code.\n");
}


#ifdef TRACK_PAGE_PROTECTION_FAULT
#include "signal_handler.hpp"
void segv_handle_func(int signum, siginfo_t * siginfo, void * context)
{
    (void)(context);
    (void)(signum);
    void * addr = siginfo->si_addr; // address of access
    if (siginfo->si_code == SEGV_ACCERR) {
        get_transaction_manager()->handle_write(addr);
    } else if (siginfo->si_code == SEGV_MAPERR) {
        fprintf(stderr, "SEGV_MAPERR\n");
        abort();
    } else {
        fprintf(stderr, "UNKNOWN ERROR\n");
        abort();
    }
}
static void inline init_sigsegv_handler( )
{
    struct sigaction siga;
    sigemptyset(&siga.sa_mask);
    sigaddset(&siga.sa_mask, SIGSEGV);
    
    sigprocmask(SIG_BLOCK, &siga.sa_mask, NULL);
    
    siga.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESTART | SA_NODEFER;
    siga.sa_sigaction = segv_handle_func;
    
    if (sigaction(SIGSEGV, &siga, NULL) == -1) {
        perror("sigaction(SIGSEGV)");
        exit(-1);
    }
    
    sigprocmask(SIG_UNBLOCK, &siga.sa_mask, NULL);
    
    fprintf(stderr, "sigsegv handler init done.\n");
}
#endif

std::atomic<int> committers;



std::thread::id MAIN_THREAD_ID;
bool MAIN_THREAD_ID_initialized = false;


void myinit();

static void initialize() __attribute__((constructor));
static void initialize(){
    init_real_functions();
    get_transaction_manager()->initialize();
    dthreadHeap::initialize();
    
    
    
    FILE *fp;
    char buff[255];
    
    fprintf(stderr, "Loading PMThreads configurations.\n");
    
    int config_nvm_latency = 0;
    int config_commit_interval = 0;
    int CPU_FREQ_MHZ = 2000;


    fprintf(stderr, "Loaded %s %s %s\n", getenv("NVM_LATENCY"), getenv("PERSIST_INTERVAL"), getenv("CPU_FREQ_MHZ"));

    config_nvm_latency = atoi(getenv("NVM_LATENCY"));
    config_commit_interval = atoi(getenv("PERSIST_INTERVAL"));
    CPU_FREQ_MHZ = atoi(getenv("CPU_FREQ_MHZ")); 

    fprintf(stderr, "Configurations have been loaded.\n");
    __start = std::chrono::steady_clock::now();
    
    Logger::initialize();


    delaycount = config_nvm_latency*CPU_FREQ_MHZ/1000;
    COMMIT_INTERVAL = config_commit_interval;
    

    fprintf(stderr, "Runtime Configurations:\n");
    fprintf(stderr, "\tlatency %d ns, delay count %d\n", config_nvm_latency, delaycount);
    fprintf(stderr, "\tCOMMIT_INTERVAL set as %d ms\n", COMMIT_INTERVAL);
    
    
    myinit();
    
    __mylib_initialized__ = true;
    
    committers.store(0, std::memory_order_release);
    
#ifdef TRACK_PAGE_PROTECTION_FAULT
    fprintf(stderr, "TRACK_PAGE_PROTECTION_FAULT\n");
    
    init_sigsegv_handler();
    
    get_transaction_manager()->_tx_begin();
#endif
    
    
    
    
}



static void finalize() __attribute__((destructor));
static void finalize(){
    get_transaction_manager()->finalize();
}


#define cpu_relax() asm volatile("pause\n": : :"memory")

extern "C" {
    
    HL::PosixLockType xx_m;
    
    void * xxmalloc (size_t sz) {
        if (__mylib_initialized__) {
            //work in shadow layer
#ifdef TRACK_PAGE_PROTECTION_FAULT
            xx_m.lock();
#endif
            void * ptr = dthreadHeap::malloc (sz);
            if (ptr == nullptr) {
                fprintf(stderr, "INTERNAL FAILURE.\n");
                ::abort();
            }
#ifdef TRACK_PAGE_PROTECTION_FAULT
            
            xx_m.unlock();
#endif
            
            return ptr;
        }
        void * ptr = initBufferPtr;
        initBufferPtr += sz;
        if (initBufferPtr > initBuffer + MAX_LOCAL_BUFFER_SIZE) {
            wzw_assert(0, "internal\n");
            abort();
        }
        {
            static bool initialized = false;
            if (!initialized) {
                initialized = true;
            }
        }
        return ptr;
    }
    
    
    
    
    size_t xxmalloc_usable_size (void * ptr) {
        //void* intPtr = get_transaction_manager()->get_internal_ptr(ptr);
        return dthreadHeap::getSize (ptr);
    }
    
    void xxfree (void * ptr) {
#ifdef TRACK_PAGE_PROTECTION_FAULT
        xx_m.lock();
#endif
        if (__mylib_initialized__) {
            dthreadHeap::free (ptr);
        }
#ifdef TRACK_PAGE_PROTECTION_FAULT
        xx_m.unlock();
#endif
    }
    
    
    
    void xxmalloc_lock() {
        // Undefined for Hoard.
        xx_m.lock();
    }
    
    void xxmalloc_unlock() {
        // Undefined for Hoard.a
        xx_m.unlock();
    }
    
    void* nvmalloc_root(size_t sz, const char* handler){
        
        void* ptr = xxmalloc(sz);
        
        if(ptr != NULL){
            int s = get_transaction_manager()->install_handler(handler, ptr);
            assert(s==0);
            (void)(s);
        }
        
        return ptr;
    }
    


    
    void install_pm_handle(const char* handle, void* ptr){
        get_transaction_manager()->install_handler(handle, ptr);
    }
    
    
    void* pmalloc(size_t sz){
        return xxmalloc(sz);
    }



    void* nvmalloc(size_t sz, const char* handler){
        (void) (handler);
        return pmalloc(sz);
    }
    
    void* pm_recover(char* handler){
        
        return get_transaction_manager()->pm_recover(handler);
        
    }

    void pm_free(void* ptr){
        xxfree(ptr);
    }
    
    void nvfree(void* ptr){
        xxfree(ptr);
    }
    

    
#ifndef TRACK_PAGE_PROTECTION_FAULT
    ssize_t read(int fd, void *buf, size_t count) {
        if(in_transaction){
            if(get_transaction_manager()->inheap(buf)){
                get_transaction_manager()->handle_sysread(buf, count);
            }
        }
        return WRAP(read)(fd, buf, count);
    }
    void* memmove ( void* dest, const void* src, size_t n ){
        if(in_transaction){
            if(get_transaction_manager()->inheap(dest)){
                get_transaction_manager()->handle_sysread(dest, n);
            }
        }
        return WRAP(memmove)(dest, src, n);
    }
    void *memcpy(void *dest, const void *src, size_t n){
        if(in_transaction){
            if(get_transaction_manager()->inheap(dest)){
                get_transaction_manager()->handle_sysread(dest, n);
            }
        }
        return WRAP(memcpy)(dest, src, n);
    }
#else

    size_t fread ( void * ptr, size_t size, size_t count, FILE * stream ){
    
        if(get_transaction_manager()->inheap(ptr)){
            get_transaction_manager()->handle_sysread(ptr, size*count);
        }
        
        return WRAP(fread)(ptr, size, count, stream);
    
    }
    
    ssize_t read(int fd, void *buf, size_t count) {
        //if(in_transaction){
            if(get_transaction_manager()->inheap(buf)){
                get_transaction_manager()->handle_sysread(buf, count);
            }
        //}
        return WRAP(read)(fd, buf, count);
    }
    void* memmove ( void* dest, const void* src, size_t n ){
        //if(in_transaction){
            if(get_transaction_manager()->inheap(dest)){
                get_transaction_manager()->handle_sysread(dest, n);
            }
        //}
        return WRAP(memmove)(dest, src, n);
    }
    void *memcpy(void *dest, const void *src, size_t n){
        //if(in_transaction){
            if(get_transaction_manager()->inheap(dest)){
                get_transaction_manager()->handle_sysread(dest, n);
            }
        //}
        return WRAP(memcpy)(dest, src, n);
    }

    
#endif
    
    
    void nvm_store(void *addr, size_t sz)
    {
        
#ifndef ZERO_TEST
        if(unlikely(in_transaction)){
            if(get_transaction_manager()->inheap(addr)){
                logger.append_addr(addr, sz);
            }
        }
#endif
    }
    
    bool isCrashed()
    {
        return get_transaction_manager()->isCrashed();
    }
    
}

unsigned int n_alive_threads = 1;
typedef struct barrier {
    pthread_cond_t complete;
    pthread_mutex_t mutex;
    int count;
    unsigned int crossing;
} barrier_t;
void barrier_init(barrier_t *b, int n);



void barrier_init(barrier_t *b, int n)
{
    pthread_cond_init(&b->complete, NULL);
    pthread_mutex_init(&b->mutex, NULL);
    b->count = n;
    b->crossing = 0;
}

int inline int_pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex){
    return WRAP(pthread_cond_wait)(cond, mutex);
}

pthread_mutex_t glock = PTHREAD_MUTEX_INITIALIZER;

#include <sys/time.h>


bool have_committed = false;

unsigned timeout_waiters = 0;

unsigned long timeInMs = COMMIT_INTERVAL * 2;
struct timeval tv;
struct timespec ts;

void inline pmthreads_barrier_cross(barrier_t *b, bool isExit)
{
    WRAP(pthread_mutex_lock)(&glock);
    if(b->crossing==0){
        have_committed = false;
        gettimeofday(&tv, NULL);
        
        
#ifdef STAT_BYTES
        enter_barrier = std::chrono::steady_clock::now();
#endif

    }
    if(isExit){
        n_alive_threads--;
    }else{
        b->crossing++;
    }
    /* If not all here, wait */
    if (b->crossing < n_alive_threads ) {
#ifdef NO_TIMED_WAIT
        int_pthread_cond_wait(&b->complete, &glock);
#else
        ts.tv_sec = time(NULL) + timeInMs / 1000;
        ts.tv_nsec = tv.tv_usec * 1000 + 1000 * 1000 * (timeInMs % 1000);
        ts.tv_sec += ts.tv_nsec / (1000 * 1000 * 1000);
        ts.tv_nsec %= (1000 * 1000 * 1000);
        pthread_cond_timedwait(&b->complete, &glock, &ts);
        if(have_committed){

        }else{
            timeout_waiters ++;
            if(timeout_waiters == b->crossing){
                //fprintf(stderr, "timoedout: %u total alive: %u \n", timeout_waiters, n_alive_threads);
                b->crossing = 0;
                timeout_waiters = 0;
                have_committed = true; //lie to them, or they will wait endlessly...
#ifdef STAT_BYTES
                exit_barrier = std::chrono::steady_clock::now();
                auto duration_cross_barrier = exit_barrier - enter_barrier;
                double time_cross_barrier = std::chrono::duration <double, std::milli> (duration_cross_barrier).count();
#endif
                pthread_cond_broadcast(&b->complete);
                
            }else{
                int_pthread_cond_wait(&b->complete, &glock);

            }
        }
#endif
    } else {
#ifdef STAT_BYTES
        exit_barrier = std::chrono::steady_clock::now();
        auto duration_cross_barrier = exit_barrier - enter_barrier;
        double time_cross_barrier = std::chrono::duration <double, std::milli> (duration_cross_barrier).count();
        fprintf(stderr, "cross_barrier cost %lf ms\n", time_cross_barrier);
#endif  
        get_transaction_manager()->_tx_commit();
        b->crossing = 0;
        have_committed = true;
        timeout_waiters = 0;
        pthread_cond_broadcast(&b->complete);
        
    }
    WRAP(pthread_mutex_unlock)(&glock);
}

barrier_t mybarrier;



void inline __quit_active_list__(){
    WRAP(pthread_mutex_lock)(&glock);
    n_alive_threads--;
    if(mybarrier.crossing == n_alive_threads)
    {
        if(n_alive_threads > 0){
           
            
#ifdef STAT_BYTES
            exit_barrier = std::chrono::steady_clock::now();
            auto duration_cross_barrier = exit_barrier - enter_barrier;
            double time_cross_barrier = std::chrono::duration <double, std::milli> (duration_cross_barrier).count();
            fprintf(stderr, "cross_barrier cost %lf ms\n", time_cross_barrier);
#endif
            get_transaction_manager()->_tx_commit();
            mybarrier.crossing = 0;
            have_committed = true;
            timeout_waiters = 0;
            pthread_cond_broadcast(&mybarrier.complete);
        }
    }else{
        //lucky guys need to do nothing.
    }
    WRAP(pthread_mutex_unlock)(&glock);
}

void inline __join_active_list__(){
    WRAP(pthread_mutex_lock)(&glock);
    n_alive_threads++;
    WRAP(pthread_mutex_unlock)(&glock);
}

void myinit(){
    barrier_init(&mybarrier, 0);
    fprintf(stderr, "my init function. \n");
}
thread_local bool update_duration_begin = true;

extern "C"
{

    
    int inline __pthread_mutex_lock(pthread_mutex_t *mutex, bool is_trylock){
        (void)(is_trylock);
        __locks_acquired__++;
        
        if(__locks_acquired__ == 1){
            in_transaction = true;
        }
        
        if(__locks_acquired__ == 1 && update_duration_begin==true){
            duration_tx_begin = std::chrono::steady_clock::now();
            update_duration_begin = false;
        }
        return WRAP(pthread_mutex_lock)(mutex);
        
        
    }
    
    int pthread_mutex_lock(pthread_mutex_t *mutex){
        return __pthread_mutex_lock(mutex, false);
    }
    
    
    int pthread_mutex_trylock(pthread_mutex_t *mutex) {
        return __pthread_mutex_lock(mutex, true);
                return -1;
    }
    
    
    
    
    
    
    int pthread_mutex_unlock(pthread_mutex_t *mutex){
        int ret = -1;
        __locks_acquired__--;
        
        ret = WRAP(pthread_mutex_unlock)(mutex);
        
        if(__locks_acquired__ == 0){
            
#ifndef PARSEC_TEST
            in_transaction = false;
#else
            in_transaction = false;
#endif
            duration_tx_end = std::chrono::steady_clock::now();
            auto diff = duration_tx_end - duration_tx_begin;
            double time_elapsed = std::chrono::duration <double, std::milli> (diff).count();
            
            if( time_elapsed > COMMIT_INTERVAL){
                
                update_duration_begin  = true;
                
                pmthreads_barrier_cross(&mybarrier, false);
                
            }
            
        }
        return ret;
        
    }
    
    typedef struct the_struct {
        void *(*start_routine) (void *);
        void * start_arguments;
    } xxx_t;
    
 
    void inline __thread_exit_call_back(){
        __quit_active_list__();


        
        
    }
    
    void pmthreads_sleep(unsigned long ms){
        __quit_active_list__();
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        __join_active_list__();
    }
    
    
    void * __thread_start (void * args)
    {
        xxx_t * xxx = (xxx_t *)(args);
        void *(*fn) (void *) = xxx->start_routine;
        void * arguments = xxx->start_arguments;
        free(xxx);
        void* ret = fn(arguments);
        __thread_exit_call_back();
        return ret;
    }
    
    int pthread_create(pthread_t *tid, const pthread_attr_t *attr, void* (*fn)(void *), void *arg){
        
        __join_active_list__();
        
        xxx_t * xxx = (xxx_t*)malloc(sizeof(xxx_t));
        xxx->start_routine = fn;
        xxx->start_arguments = arg;
        
        return WRAP(pthread_create)(tid, attr, __thread_start, xxx);
    }
    
    
    
    int pthread_join(pthread_t tid, void **val) {

        __quit_active_list__();
        
        int ret = WRAP(pthread_join)(tid, val);

        __join_active_list__();

        
        return ret;
    }
    
    
    int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex){
        
        
        
        __quit_active_list__();
        
        //fprintf(stderr, "------>cond wait \n");
        WRAP(pthread_cond_wait)(cond, mutex);
        //fprintf(stderr, "------>cond wake \n");
        __join_active_list__();
        
        
        
        
        return 0;
        
    }
    
    
    int pthread_cond_broadcast(pthread_cond_t * cond){
        //fprintf(stderr, "%lu broadcast intercepted\n", std::this_thread::get_id());
        
        return WRAP(pthread_cond_broadcast)(cond);
    }
    
    int pthread_cond_signal(pthread_cond_t * cond) {
        //fprintf(stderr, "signal intercepted\n");
        return WRAP(pthread_cond_signal)(cond);
    }
    
    
    
    //WRAP(
    int pthread_cancel(pthread_t thread){
        fprintf(stderr, "cancel?\n"); 

		::abort();
        return WRAP(pthread_cancel)(thread);
    }
    
    void pthread_exit(void *value_ptr){
        fprintf(stderr, "exit?\n");
        ::abort();
		WRAP(pthread_exit)(value_ptr);
    }

    
}
