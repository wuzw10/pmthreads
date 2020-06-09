#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_options.h"
#include "barrier.hpp"
#include <random>
#include <atomic>
#include <chrono>
#include <list>
#include <thread>
#include <sys/mman.h>
#include <iostream>
#include <fstream>
#include "coupling.h"

#include "nvrecover.h"
#include "coupling.h"
#define BEGIN() pthread_mutex_lock(&mtx)
#define END() pthread_mutex_unlock(&mtx)
typedef int64_t entry_key_t;

pthread_mutex_t mtx;

typedef struct thread_args{
    int tid;
    barrier_t * barrier;
    options_t * options;
}thread_data;

std::default_random_engine generator;
std::uniform_int_distribution<unsigned long> distribution(0, 1000000000000);
std::uniform_int_distribution<unsigned long> distribution2(0, 100);
std::uniform_real_distribution<float> ratio_distribution(0, 1.0);
std::atomic<bool> __stop__;

barrier_t barrier;



entry_key_t*  keys = NULL;
intset_l_t* ll = NULL;




void* thr_func(void* args){
    thread_data* targs = (thread_data*)args;
    int tid = targs->tid;
    options_t* opts = targs->options;
    barrier_cross(targs->barrier);
    
    printf("T%d barrier crossed\n", tid);
    
    
    unsigned long num = _options.num_entries;
    
    
    unsigned long len = num / _options.num_threads;
    
    unsigned long start = tid*len;
    unsigned long end = start + len;
    
    if(!is_crashed()){
        printf("T%d not crashed, will insert %lu elements. \n", tid, len);
        //BEGIN();
        int i = start;
        //END();
        while(!__stop__.load(std::memory_order_relaxed)){
            lockc_insert(ll, keys[i]);
            i++;
            if(i==end) break;
        }
    }else{
    }
    
    
    printf("T%d exit.\n", tid);
    
    return NULL;
}


void load_input_data();


int main(int argc, char *argv[])
{
    load_input_data();
    pthread_mutex_init(&mtx, NULL);
    get_options(argc, argv);
    int num_threads = _options.num_threads;
    pthread_t* threads = new pthread_t[num_threads];
    thread_data * data = new thread_data[num_threads];
    barrier_init(&barrier, num_threads + 1); //worker_threads + main_thread
    
    for (int i=0; i<num_threads; i++) {
        data[i].barrier = &barrier;
        data[i].tid = i;
        data[i].options = &_options;
        pthread_create(&threads[i], NULL, thr_func, &data[i]);
    }
    
    __stop__.store(false, std::memory_order_release);
    /******************************************************/
    
    node_l_t *min, *max;
    
    if(!is_crashed()){
        BEGIN();
        
        ll = (intset_l_t*)nvmalloc_root(sizeof(intset_l_t), "set");
        
        printf("addr ll: %p\n", (void*)ll);
        
        max = (node_l_t *)nvmalloc(sizeof(node_l_t), "max");
        max->val = VAL_MAX;
        max->next = NULL;
        INIT_LOCK(&(max->lock));
        min = (node_l_t *)nvmalloc(sizeof(node_l_t), "min");
        min->val = VAL_MIN;
        min->next = max;
        INIT_LOCK(&(min->lock));
        ll->head = min;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        END();
    }else{
        BEGIN();
        ll = (intset_l_t*)pm_recover("set");
        printf("addr ll: %p\n", (void*)ll);
        
        
        END();
    }
    /******************************************************/
    barrier_cross(&barrier);
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(_options.duration));
    
    
    
    //Timed Abort
    
    if(!is_crashed()){
        //quit ative list
        //sleep
        //rejoin active list
        pmthreads_sleep(_options.duration); //Enable commit
        
        
        __stop__.store(true, std::memory_order_release);
        
        for (int i=0; i<num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        
        size_t sz = set_size_l(ll);
        fprintf(stderr, "#successfully inserted DRAM elements: %ld\n", sz);
        
        
        //sleep random (0, 100) ms
        
        unsigned long tt = distribution2(generator);
        assert(tt>=0 && tt<=100);
        pmthreads_sleep(tt);
        
        
        //__stop__.store(true, std::memory_order_release);
        abort();
    }
    
    
    for (int i=0; i<num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    int sz = set_size_l(ll);
    printf("#successfully inserted keys: %d\n", sz);
    
    node_l_t* curr = ll->head;
    auto dram_recover_start = std::chrono::steady_clock::now();
    while(curr != NULL){
        INIT_LOCK(&(curr->lock));
        //printf("%ld \t", curr->val);
        curr = curr->next;
    }
    auto duration = std::chrono::steady_clock::now() - dram_recover_start;
    double duration_dram_recover = std::chrono::duration <double, std::milli> (duration).count();
    fprintf(stderr, "dram recovering takes %lf ms.\n", duration_dram_recover);
    
    printf("\n");
    printf("Main exit.\n");
    
    delete []data;
    delete []threads;
    return 0;
}


void load_input_data(){
    int nkeys = 1000000;
    assert(nkeys <= 1000000);
    keys = new entry_key_t[nkeys];
    std::ifstream ifs;
    ifs.open("./uniform_input.txt");
    if(!ifs){
        std::cout << "input loading error!" << std::endl;
        exit(-1);
    }
    for(unsigned i=0; i<nkeys; i++){
        ifs >> keys[i];
    }
    fprintf(stderr, "Finish warming up. \n");
}

