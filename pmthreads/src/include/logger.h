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
#ifndef ilog_h
#define ilog_h
#include <stdlib.h> //for posix_memalign
#include <stdio.h>
#include <unordered_set>
#include <pthread.h>
#include <atomic>
#include "real.h"
#include "debug.h"
#include "locks/posixlock.h"
class Logger;
#define max_threads 256
extern Logger* entries[max_threads];

#include "wrappers/stlallocator.h"
#include "internal_heap.h"

class Logger{
public:
	typedef void* word;
#ifdef TRACK_PAGE_PROTECTION_FAULT
    std::unordered_set<void*, std::hash<void*>, std::equal_to<void*>, HL::STLAllocator<void*, internal_heap>> dirtySet;
#else
    std::unordered_set<word> dirtySet;
#endif
    Logger(){
        DRAM_written = 0;
		id = -1;
		lock();
		for(int i=0;i<max_threads;i++){
			if(entries[i]==NULL){
				entries[i] = this;
				id = i;
				break;
			}
		}
		if(id==-1){
			fprintf(stderr, "internal error\n");
			abort();
		}
		unlock();
	}

	~Logger(){
		lock();
		entries[id] = NULL;
		unlock();
	}
	
	inline int get_id(){
		return id;
	}

	static inline void lock(){
		_lock.lock();
	}

	static inline void unlock(){
		_lock.unlock();
	}

	static void initialize(){
		for(int i=0; i<max_threads; i++)
			entries[i] = NULL;
	}

	inline void append_addr(void* addr, size_t sz){
		unsigned long page_addr = (unsigned long)addr & PAGE_MASK;
		dirtySet.emplace((void*) page_addr);
        
		DRAM_written += sz;
        
		(void)(sz);
	
	}

	inline void clear(){
		dirtySet.clear();
        DRAM_written = 0;
	}
    unsigned long DRAM_written;
private:
	static const size_t PAGE_SIZE = 4096;
	static const size_t PAGE_MASK =  (~(PAGE_SIZE-1));
	int id;
    
	static HL::PosixLockType _lock;
};
#endif /* ilog_h */
