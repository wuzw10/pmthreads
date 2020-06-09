// -*- C++ -*-

/*
Author: Emery Berger, http://www.cs.umass.edu/~emery

Copyright (c) 2007-8 Emery Berger, University of Massachusetts Amherst.

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

/*
 * @file   xheap.h
 * @brief  A basic bump pointer heap, used as a source for other heap layers.
 * @author Emery Berger <http://www.cs.umass.edu/~emery>
 * @author Tongping Liu <http://www.cs.umass.edu/~tonyliu>
 */

#ifndef DTHREADS_XHEAP_H
#define DTHREADS_XHEAP_H
#include <assert.h>
#include "xdefines.h"
#include "debug.h"
#include "Heap-Layers/locks/posixlock.h"
#include "pmthreads.h"
extern transaction_manager* get_transaction_manager();

class xheap{
	public:

	enum { Alignment = 4 * 1024UL };
	// It is very very important to put the first page in a separate area since it will invoke a
	// lot of un-necessary rollbacks, which can affect the performance greately and also can affect
	// the correctness of race detection, since different threads can end up getting the same memory
	// block from class "xheap".
	// Although we can rely on "rollback" mechanism to avoid the problem,
	// but we still need to differentiate those races caused by this and actual races in user application.
	// It is not good to do this.
	// It is possible that we don't need sanityCheck any more, but we definitely need a lock to avoid the race on "metatdata".
	xheap() {
		// Since we don't know whether shareheap has been initialized here, just use mmap to assign
		// one page to hold all data. We are pretty sure that one page is enough to hold all contents.
		init();
	}
	void initialize(){
		//DEBUG("xheap init");
	}
	inline void init(){
		_magic = 0xCAFEBABE;
		_position = get_transaction_manager()->_start_pos();
		_remaining = get_transaction_manager()->size();
		//DEBUG("xheap, start from: %p, total size: %lu", _position, _remaining);
	}
	
	inline void * getend() {
		//get_transaction_manager()->mylock("xheap");
		void* res = _position;
		//get_transaction_manager()->myunlock("xheap");
		return res;
	}

	// We need to page-aligned size, we don't need two different threads are using the same page here.
	inline void * malloc(size_t sz) {
		sanityCheck();

		// Roud up the size to page aligned.
		sz = xdefines::PageSize * ((sz + xdefines::PageSize - 1)
				/ xdefines::PageSize);

		//get_transaction_manager()->mylock("xheap");
		_lock.lock();
			
		if (_remaining == 0) {
			fprintf(stderr, "Try to change PROTECTEDHEAP_SIZE in xdefine.h to a bigger value\n");
			exit(-1);
		}

		if (_remaining < sz) {
			fprintf(stderr, "Try to change PROTECTEDHEAP_SIZE in xdefine.h to a bigger value\n");
			exit(-1);
		}
		void * p = (void *)_position;

		// Increment the bump pointer and drop the amount of memory.
		_remaining -= sz;
		_position =(void*)( (unsigned long)_position + sz);
		
		//DEBUG("remaining %lu", (unsigned long)_remaining);
		
		get_transaction_manager()->setHeapEnd(_position);

		_lock.unlock();
		//get_transaction_manager()->myunlock("xheap");

		assert ((size_t) p % Alignment == 0);

		return p;
	}

	// These should never be used.
	inline void free(void *) {
		sanityCheck();
	}
	
	inline int remove (void *) { return 0; }

	inline size_t getSize(void * ) {
		sanityCheck();
		return 0;
	} 

	private:
	void sanityCheck() {
		if (_magic != 0xCAFEBABE) {
			fprintf(stderr, "%d : WTF with magic %ld!\n", getpid(), _magic);
			::abort();
		}
	}
	void* _position;
	size_t _remaining;
	size_t _magic;
	HL::PosixLockType _lock;
};

#endif
