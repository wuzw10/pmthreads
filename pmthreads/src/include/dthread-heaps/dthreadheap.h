#ifndef _DTHREADHEAP_H
#define _DTHREADHEAP_H
#include "dthreadheap.h"
#include "xheap.h"
#include "heaps/utility/uniqueheap.h"
#include "heaps/special/zoneheap.h"
#include "heaps/top/mmapheap.h"
#include "heaps/objectrep/sizeheap.h"
#include "heaps/general/kingsleyheap.h"
#include "heaps/buildingblock/adaptheap.h"
#include "wrappers/ansiwrapper.h"
#include "utility/dllist.h"

#include "utility/modulo.h"
#include "threads/cpuinfo.h"

#include "warpheap.h"
#include "xdefines.h"
#include "xheap.h"
#include "xoneheap.h"
#include "debug.h"
//extern thread_local int _heapid;

extern thread_local Logger logger;
using namespace HL;
class dthreadHeap{
	private:
		static warpheap<xdefines::NUM_HEAPS, xdefines::PROTECTEDHEAP_CHUNK, xoneheap<xheap > > _pheap;
	public:
		static inline void *malloc(size_t sz) {
			
			int _heapid = (int)Modulo<xdefines::NUM_HEAPS>::mod (CPUInfo::getThreadId());
			//int _heapid = logger.get_id();		
			//DEBUG("heap id is %u", _heapid);
			void * ptr = _pheap.malloc((int)_heapid, sz);
			
			return ptr;
		}
		static inline void initialize(){
			_pheap.initialize();
		}
		static inline void * realloc(void * ptr, size_t sz) {
			
			size_t s = getSize(ptr);
			void * newptr = malloc(sz);
			if (newptr) {
				size_t copySz = (s < sz) ? s : sz;
				memcpy(newptr, ptr, copySz);
			}
			free(ptr);
			
			return newptr;
		}

		static inline void free(void * ptr) {
			
			int _heapid = (int)Modulo<xdefines::NUM_HEAPS>::mod (CPUInfo::getThreadId());
			
			return _pheap.free((int)_heapid, ptr);
		}

		/// @return the allocated size of a dynamically-allocated object.
		static inline size_t getSize(void * ptr) {
			
			// Just pass the pointer along to the heap.
			size_t res =  _pheap.getSize(ptr);
			
			return res;
		}
};
#endif
