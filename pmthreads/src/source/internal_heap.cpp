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
#include "internal_heap.h"

#include <stdlib.h>

#include "heaps/utility/uniqueheap.h"
#include "heaps/special/zoneheap.h"
#include "heaps/top/mmapheap.h"
#include "heaps/objectrep/sizeheap.h"
#include "heaps/general/kingsleyheap.h"
#include "heaps/buildingblock/adaptheap.h"
#include "wrappers/ansiwrapper.h"
#include "utility/dllist.h"

using namespace HL;

class TopHeap : public SizeHeap<UniqueHeap<ZoneHeap<MmapHeap, 65536> > > {};

class TheCustomHeapType :
  public ANSIWrapper<KingsleyHeap<AdaptHeap<DLList, TopHeap>, TopHeap> > {};

inline static TheCustomHeapType * getCustomHeap() {
  static char thBuf[sizeof(TheCustomHeapType)];
  static TheCustomHeapType * th = new (thBuf) TheCustomHeapType;
  return th;
}

PosixLockType internal_heap::_m;


void* internal_heap::malloc(size_t sz)
{   _m.lock();
    void * ptr = getCustomHeap()->malloc(sz);
    _m.unlock();
    return ptr;
}


void internal_heap::free(void* ptr)
{
    _m.lock();
    getCustomHeap()->free(ptr);
    _m.unlock();
}


size_t internal_heap::getSize(void* ptr)
{
    _m.lock();
    size_t  res =  getCustomHeap()->getSize(ptr);
    _m.unlock();
    return res;
}


void* internal_heap::realloc(void* ptr, size_t sz)
{
    _m.lock();
    void* res = getCustomHeap()->realloc(ptr, sz);
    _m.unlock();
    return res;
}

