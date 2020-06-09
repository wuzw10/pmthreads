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
#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

// UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include "debug.h"
#include <unordered_set>
#include "locks/posixlock.h"
#include "utility/timer.h"
#include "wrappers/stlallocator.h"
#include "internal_heap.h"
#include <string.h>
#include "logger.h"
#include "fences.h"
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include <assert.h>
#include <chrono>


#include "real.h"

#include <xmmintrin.h>//SSE_SUPPORT
#define unlikely(x)     __builtin_expect((x),0)
extern thread_local volatile bool in_transaction;

static volatile std::atomic<bool> handler_lock(false);

static auto last_commit_time  = std::chrono::steady_clock::now();
static auto kick_off_timestamp  = std::chrono::steady_clock::now();

//auto end = std::chrono::steady_clock::now();
//auto diff = end - start;
//double ms = std::chrono::duration <double, std::milli> (diff).count();


extern thread_local Logger logger;
extern Logger* entries[max_threads];
#define FNAME_LENGTH_LIMIT 256
class transaction_manager{
	public:


		/*
		 *			pageNo, version
		 *			flush
		 *			flush log header
		 * */

		void handle_sysread(void* buf, size_t count){
			//return;
			//fprintf(stderr, "handle_sysread\n");
#ifndef ZERO_TEST
			_lock.lock();
			unsigned long p = (unsigned long)(buf)&PAGE_MASK;
			unsigned long end = (unsigned long)(buf) + count;

#ifdef TRACK_PAGE_PROTECTION_FAULT
			size_t num_pages = count/4096 + 1;
			mprotect((void*)p, num_pages * 4096, PROT_READ|PROT_WRITE);
#endif


			unsigned long i=0;

			while(p<end){
				p+=i*PAGE_SIZE;
				logger.append_addr((void*)p, 4096);
				//myset.emplace(p);
				i++;
			}
			_lock.unlock();
#endif
		}

		bool inline inheap(void* addr){
			unsigned long sa = (unsigned long)(addr);
			if( sa>= _shadowBase && sa< _shadowBase+__end){
				return true;
			}else{
				return false;
			}
		}



		void inline __persist(unsigned long userPageAddr)
		{
			num_commits++;
			unsigned long offset = userPageAddr - _shadowBase;

			size_t pageNo = (offset/PAGE_SIZE);

			size_t generation = meta->generation + 1;

			int dest = (pageVersions[pageNo].version[0] > pageVersions[pageNo].version[1]) ? 1:0;
			pageVersions[pageNo].version[dest] = generation;
			sync(&(pageVersions[pageNo]));

			void* src = (void*) userPageAddr;
			void* backing = (void*)(_backingBase[dest] + offset);
			__commitPage(src, backing);

		}


#ifdef TRACK_PAGE_PROTECTION_FAULT
		inline void spin_lock(){
			while(std::atomic_exchange_explicit(&handler_lock, true, std::memory_order_acquire)){
				asm volatile("pause\n": : :"memory");
			}
		}
		inline void spin_unlock(){
			std::atomic_store_explicit(&handler_lock, false, std::memory_order_release);
		}
		inline void handle_write(void* addr){
			unsigned long ptr = (unsigned long)(addr) & PAGE_MASK;
			void* page = (void*) ptr;
			mprotect(page, PAGE_SIZE, PROT_READ|PROT_WRITE);
			if(unlikely(in_transaction)){
				if(inheap(addr)){
					logger.append_addr(addr, 4096);
				}
			}
		}
#endif


		void _tx_begin(){
#ifdef TRACK_PAGE_PROTECTION_FAULT
			_lock.lock();
			mprotect(shadowPages, __end, PROT_READ);
			_lock.unlock();
#endif
		}


		void _tx_commit(){
#ifdef STAT_BYTES
			auto duration_ = std::chrono::steady_clock::now() - last_commit_time;
			double duration_since_last_commit = std::chrono::duration <double, std::milli> (duration_).count();
			//fprintf(stderr, "Time since last commit %lf ms \n", duration_since_last_commit);        
#endif
			num_tx++;
#ifdef TRACK_PAGE_PROTECTION_FAULT
			std::unordered_set<void*, std::hash<void*>, std::equal_to<void*>, HL::STLAllocator<void*, internal_heap>> _set;
#else
			std::unordered_set<void*> _set;
#endif

			auto persist_start = std::chrono::steady_clock::now();

			unsigned long dram_written = 0;

			//commit log
			Logger::lock();
			for(int i=0; i<max_threads; i++){
				if(entries[i]!=NULL){
					Logger* log = entries[i];
					assert(log->dirtySet);

					_set.insert(log->dirtySet.begin(), log->dirtySet.end());

					dram_written += log->DRAM_written;

					log->clear();
				}
			}
			Logger::unlock();


			unsigned long commited_pages = _set.size();

			for (const auto& addr: _set) {
				//unsigned long paddr = (unsigned long)*itr;
				__persist((unsigned long)addr);
			}


			auto persist_end  = std::chrono::steady_clock::now();
			auto diff = persist_end - persist_start;
			double duration_in_ms = std::chrono::duration <double, std::milli> (diff).count();

			__persist_time += duration_in_ms;

			//updte global generation at last.
			//persistent inc generation
			meta->generation++;
			sync(&(meta->generation));

			auto duration = std::chrono::steady_clock::now() - kick_off_timestamp;
			double commit_timestamp = std::chrono::duration <double, std::milli> (duration).count();
			//fprintf(stderr, "COMMIT TIMESTAMP %lf ms \n", commit_timestamp);

#ifdef STAT_BYTES
			fprintf(stderr, "[COMMIT STATISTICS] [TIME TICKS (ms)] %lf [VOLATILE WRITES (bytes)] %ld [DURABLE WRITES (bytes)] %ld  [REDUCED WRITES (bytes)] %ld\n", commit_timestamp, dram_written, bytes_this_time, dram_written-bytes_this_time);
#endif
			__save += ((double)(dram_written-bytes_this_time))/(1024*1024);

			bytes_this_time = 0;
			dram_written = 0;

#ifdef STAT_BYTES
			//fprintf(stderr, "This time we persist %lu bytes in : %lf ms\n", bytes_this_time, duration_in_ms);
			__bytes += bytes_this_time;
			last_commit_time  = std::chrono::steady_clock::now();
			bytes_this_time = 0;
#endif

#ifdef TRACK_PAGE_PROTECTION_FAULT
			_tx_begin();
#endif


		}
		int install_handler(const char* handler, void* ptr){
			//sprintf(logFiles[0], "%s/_backingPage%d", logPath, 0);

			char hpath[FILENAME_MAX];
			sprintf(hpath, "%s/%s", logPath, handler);

			FILE *fp = fopen(hpath, "w");

			assert(fp!=NULL);
			fprintf(fp, "%p", ptr);
			fclose(fp);

			return 0;
		}


		void* pm_recover(char* handler){

			void* ptr = NULL;
			char hpath[FILENAME_MAX];
			sprintf(hpath, "%s/%s", logPath, handler);

			FILE *fp = fopen(hpath, "r");
			assert(fp != NULL);
			int res = fscanf(fp, "%p", &ptr);
			(void)(res);
			fclose(fp);
			return ptr;
		}



		inline void* _start_pos(){
			return shadowPages;
		}

		inline size_t size(){
			return __end;
		}


		void initialize(){

			if(access(fname_crash, F_OK) != -1){
				DEBUG("we crashed before");
				_crashed = true;
			}else{
				DEBUG("Normal start");
				_crashed = false;
			}

			auto recovery_start_time = std::chrono::steady_clock::now();


#ifdef NO_CRASH_TEST
			_crashed = false;
			meta = (metadata_t *)mmap(NULL, sizeof(metadata_t), PROT_READ| PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
#else
			//create crash log file
			int fd_c = open(fname_crash, O_RDWR|O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP  | S_IROTH | S_IWOTH);
			if(fd_c==-1){
				DEBUG("failed to create crash file. ");
				::abort();
			}
			if(ftruncate(fd_c, sizeof(metadata_t))){
				DEBUG("failed to set length for crash file. ");
				::abort();
			}

			meta = (metadata_t *)mmap(NULL, sizeof(metadata_t), PROT_READ| PROT_WRITE, MAP_SHARED, fd_c, 0);
#endif
			DEBUG("sizof meta %lu", sizeof(metadata_t));

			if(_crashed){
				//recover nvid
				nvid = meta->nvid;
				memcpy(logPath, meta->logPath, strlen(meta->logPath));
				DEBUG("nvid recoverd to %d", nvid);
			}else{
				//init nvid
				srand ((unsigned)time(NULL));
				nvid = rand();
				const char* root = "/dev/shm/pmthreads";
				sprintf(logPath, "%s/%d", root, nvid);
				int status = mkdir(logPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				if(status != 0){
					perror("mkdir failed.");
					abort();
				}
				meta->nvid = nvid;
				memcpy(meta->logPath, logPath, strlen(logPath));
				DEBUG("set nvid as %d", nvid);

				//init generation
				meta->generation = 0;
			}

			sprintf(logFiles[0], "%s/_backingPage%d", logPath, 0);
			sprintf(logFiles[1], "%s/_backingPage%d", logPath, 1);
			DEBUG("logPath %s", logPath);


			size_t sz = ((size_t)1<<30) * 4;
			// 1<<30  1GB
			int fd[2] = {-1, -1};
			for(int i=0; i<2; i++){
				fd[i] = open(logFiles[i], O_RDWR | O_CREAT,
						S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP  | S_IROTH | S_IWOTH );
				if(fd[i]==-1){
					DEBUG("failed to open backing file %d", i);
					::abort();
				}
				if(ftruncate(fd[i], (__off_t)sz)){
					DEBUG("failed to truncate backing file %d", i);
					::abort();
				}
				_internalPages[i] = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd[i], 0);
				if(_internalPages[i] == MAP_FAILED){
					DEBUG("failed to map %d", i);
					::abort();
				}
				_internalFd[i] = fd[i];
				_backingBase[i] = (unsigned long)_internalPages[i];
			}

			if(_crashed){
				shadowPages = mmap(meta->heapbase , sz, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

				if(shadowPages != meta-> heapbase ){
					fprintf(stderr, "failed to map heap.\n");
					abort();
				}
				DEBUG("recover shadow base at %p", shadowPages);
			}else{
				shadowPages = mmap(NULL , sz, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
				meta->heapbase = shadowPages;
				DEBUG("set shadow base at %p", shadowPages);
			}
			if(shadowPages == MAP_FAILED){
				DEBUG("create shadowing memory failed.");
				::abort();
			}
			_shadowBase = (unsigned long)shadowPages;

			//for both fresh run and recovered run, just assign.
			pageVersions = meta->pageVersions;


			//recover pages (remmap)
			if(_crashed){
				/*TODO remap shadow space according to recovered page versions
				 *  remap shadow space ...
				 */
				size_t len = ((size_t)(meta->heapend) - (size_t)(meta->heapbase))/PAGE_SIZE;
				DEBUG("%lu/%lu pages to be remapped ", len, num_pages);

				assert(len <= num_pages);

				size_t last_generation = meta->generation;

				for(size_t i=0; i<len; i++){
					pageVersionType* p = &(pageVersions[i]);
					if(p->version[0] || p->version[1])
						fprintf(stderr, "++++++%lu last gen:%lu\n \t%lu\n \t%lu\n", i, last_generation, p->version[0], p->version[1]);

					int valid = 0;
					if(p->version[0] || p->version[1]){ // not all zero
						int max = (p->version[0] >= p->version[1]) ? 0 : 1;
						if(p->version[max] > last_generation){
							valid = ~max;
							assert(p->version[valid] <= last_generation);
						}else{
							valid = max;
						}
						DEBUG("version[%d]=%lu is valid for page %lu", valid, p->version[valid], i);

						//have no checkpoint, nothing to recover
						if(p->version[valid] ==0 ){
							continue;
						}

						off_t offset = i*PAGE_SIZE;
						void * page = (void*)( _shadowBase + offset);
						void * old = page;
						DEBUG("remap page %lu to backing %d", i, valid);
						page = mmap(page, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, _internalFd[valid], offset);
						if(page != old){
							fprintf(stderr, "remap failed. \n");
							abort();
						}
					}else{
						//both versions are zero, no commits, just skip remapping.
					}
				}
			}


			if(_crashed){
				auto duration = std::chrono::steady_clock::now() - recovery_start_time;
				double recovery_duration = std::chrono::duration <double, std::milli> (duration).count();
				fprintf(stderr, "precovery takes %lf mili seconds.\n", recovery_duration);
			}


			__save = 0;


			__end = sz;
			num_tx = 0;
			num_commits = 0;


			bytes_this_time = 0;
			__bytes = 0;
			__persist_time = 0;
			allones = _mm_setzero_si128();
			allones = _mm_cmpeq_epi32(allones, allones);

			kick_off_timestamp  = std::chrono::steady_clock::now();

			DEBUG("=========INITIALIZE==========\n");
		}

		inline void setHeapEnd(void* pos){
			meta->heapend = pos;
			sync(meta->heapend);
		}

		inline bool isCrashed(){
			return _crashed;
		}

		void finalize(){
#ifdef TEST_ONLY
			_tx_commit();
#endif
			//clear log files on normal exit;
			//unlink(logFiles[0]);
			//unlink(logFiles[1]);
			//rmdir(logPath);

			DEBUG("\n\n=======FINALIZE========")
				if(unlink(fname_crash)){
					DEBUG("failed to unlink crash log");
				}else{
					DEBUG("Normal exit, rmove crash flag.");
				}
			fprintf(stderr, "persist %lu pages ( %lu times, %lu bytes) in %lf ms.\n\n", num_commits, num_tx, __bytes, __persist_time);
			DEBUG("num_tx %lu .", num_tx);
			DEBUG("commiy %lu .", num_tx);
#ifdef STAT_BYTES
			fprintf(stderr, "[AVERAGE REDUCED WRITES] %lf MB.\n", ((double)(__save))/num_tx);
#endif
		}

		transaction_manager(){

		}
		~transaction_manager(){
		}

	private:
		unsigned long __save;
		unsigned long __bytes;
		//recovery
		const char* fname_crash = "./.crashed.log";
		int nvid;
		bool _crashed;
		char logPath[FILENAME_MAX];
		char logFiles[2][FILENAME_MAX];
		static const size_t PAGE_SIZE = 4096;
		static const size_t heap_sz = 4* ((size_t)1<<30);
		static const size_t num_pages = heap_sz/PAGE_SIZE;
		typedef struct pageversion{
			size_t version[2];
		}pageVersionType;
		class metadata_type{
			public:
				int nvid;
				char logPath[FILENAME_MAX];
				void* heapbase;
				void* heapend;
				size_t generation;
				pageVersionType pageVersions[num_pages];
		};
		typedef metadata_type metadata_t;
		metadata_t * meta;

		typedef long long compareType;
		typedef unsigned long KeyType;
		typedef HL::STLAllocator<KeyType, internal_heap> setAllocator;
		typedef std::unordered_set<KeyType, std::hash<KeyType>, std::equal_to<KeyType>, setAllocator> SetType;
		SetType myset;
		SetType dirtyPageSet;



		size_t num_tx;

		double __persist_time;
		unsigned long num_commits;
		unsigned long _shadowBase;
		unsigned long _backingBase[2];
		size_t __end;
		int _internalFd[2];
		void* _internalPages[2];
		void* shadowPages;
		HL::PosixLockType dirtyListLock;
		//add global lock
		HL::PosixLockType _lock;
		unsigned long bytes_this_time = 0;

		pageVersionType * pageVersions;

		//---------define zone-------------//
		static const size_t PAGE_MASK =  (~(PAGE_SIZE-1));
		enum { PAGE_SIZE_MASK = (PAGE_SIZE-1) };

		inline void commitWord(char * src, char * twin, char * dest) {
			for(size_t i = 0; i < sizeof(compareType); i++) {
				if(src[i] != twin[i]) {
					dest[i] = src[i];
					//clflushopt(&(dest[i]));
					//delays();
				}
			}
		}

		__m128i allones;

		inline void __commitPage(const void* src, const void* dest)
		{
#ifdef STAT_BYTES
			char * _s = (char*)src;
			char * _d = (char*)dest;
			for(int i=0; i<4096; i++){
				if(_s[i] != _d[i]){
					++bytes_this_time;
				}
			}

#endif

#ifdef USE_CLFLUSH_TO_COMMIT
			size_t start = 0;
			size_t end = PAGE_SIZE;
			int items_per_cl = 64/sizeof(compareType);
			int is_cl_dirty =0;
			compareType* local = (compareType*) src;
			compareType* share = (compareType*) dest;
			//assert((local & PAGE_SIZE_MASK ==0) && (share & PAGE_SIZE_MASK == 0));
			for(size_t i = start; i < (end-start)/sizeof(compareType); i++){
				if(local[i] != share[i]) {
					share[i] = local[i];
					is_cl_dirty = 1;
				}

				if(i%items_per_cl == (items_per_cl -1) ){   
					if(is_cl_dirty){
						clflush(&(share[i - items_per_cl + 1 ]));
						delays();
						is_cl_dirty = 0;
					}
				}
			}
#else

			//cacheline size = 64 bytes
			//        
			//        char * _s = (char*)src;
			//        char * _d = (char*)dest;
			//        for(int i=0; i<4096; i++){
			//            if(_s[i] != _d[i]){
			//                __bytes++;
			//            }
			//        }
			//inject delays as a function of the diff cachelines
			int diffcls = 0;
			long long  * a = (long long*)src;
			long long * b = (long long*)dest;
			for(int cl=0; cl<4096/64; cl++){
				for(size_t k=0; k<64/(sizeof(long long)); k+=sizeof(long long)){
					size_t i = cl*64 + k;
					if(a[i]!=b[i]){
						diffcls++;
						break;
					}
				}
			}
			//fprintf(stderr, "diff cls is %d\n", diffcls);
			while(diffcls--){
				delays();
			}

			__m128i * localbuf = (__m128i *) src;
			__m128i * twinbuf = (__m128i *) dest;
			__m128i * destbuf = (__m128i *) dest;

			//#pragma vector always
			for (unsigned i = 0; i < PAGE_SIZE/ sizeof(__m128i ); i++){

				__m128i localChunk, twinChunk;
				localChunk = _mm_load_si128(&localbuf[i]);
				twinChunk = _mm_load_si128(&twinbuf[i]);

				__m128i eqChunk = _mm_cmpeq_epi8(localChunk, twinChunk);
				__m128i neqChunk = _mm_xor_si128(allones, eqChunk);
				/*
				   _mm_maskmoveu_si128
				   Conditionally store 8-bit integer elements from a into memory using mask (elements are not stored when the highest bit is not set in the corresponding element) and a 

				   [non-temporal memory hint]. 

				   mem_addr does not need to be aligned on any particular boundary.
				   */
				_mm_maskmoveu_si128(localChunk, neqChunk, (char *) &destbuf[i]);
				//we need to inject delays conditionally.

				//delays(); //see upwards, the refined delays function.

			}
#endif
		}
		inline size_t _pageNo(unsigned long addr){
			unsigned long paddr = addr&PAGE_MASK;
			unsigned long offset = paddr - _shadowBase;
			return  (offset)/PAGE_SIZE;
		}
};
#endif 
