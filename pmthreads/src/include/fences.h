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
#ifndef fences_h
#define fences_h
#define CFENCE asm volatile ("":::"memory")
#ifdef __APPLE__
#define FLUSH(addr) clflush(addr)
#define DRAIN()
#else
#define FLUSH(addr) clflush(addr)
#define DRAIN() sfence();
#endif


//TODO emulate clwb with latency


static inline void clflush(volatile void *__p)
{
    asm volatile("clflush %0" : "+m" (*(volatile char *)__p));
}
static inline void clflushopt(volatile void *__p){
    asm volatile(".byte 0x66; clflush %0" : "+m" (*(volatile char *)__p));
}
static inline void asm_movnti(volatile unsigned long *addr, unsigned long val)
{
    __asm__ __volatile__ ("movnti %1, %0" : "=m"(*addr): "r" (val));
}
static inline void sfence(){
    asm volatile ("sfence":::"memory");
}
static inline void mfence(){
    asm volatile ("mfence":::"memory");
}
static inline unsigned long long rdtsc(void)
{
    unsigned int hi, lo;
    __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}
extern int delaycount;
#define DELAYSFUNC inline
//#define DELAYSFUNC __attribute__((noinline))

static DELAYSFUNC void delays(){
    unsigned long long t=rdtsc()+ (unsigned long long)delaycount;
    while(1){
        asm volatile ("pause":::"memory");
        if(rdtsc()>t) break;
    }
}
static inline void sync(volatile void *__p){
    //mfence();
    FLUSH(__p);
    delays();
    asm volatile ("mfence":::"memory");
}

typedef size_t word;


//flush the range [start, end)
static inline void sync_range(volatile void *start, volatile void *end){
    //mfence();
    FLUSH(start);
    volatile void *pp = (volatile void *)(((word)(start)|63)+1); //beginning of the next cache line.
    for(; pp<end; pp=(volatile void *)((char*)pp+64))
        FLUSH(pp);
    delays();
    DRAIN();
}
static inline void sync2(void *start, void *end, void *other){
    FLUSH(start);
    volatile void *pp = (volatile void *)(((word)(start)|63)+1); //beginning of the next cache line.
    for(; pp<end; pp=(volatile void *)((char*)pp+64))
        FLUSH(pp);
    FLUSH(other);
    delays();
    DRAIN();
}
static inline void memsetblocknodrain(void *start, void *end, word pattern){
    volatile unsigned long *ss=(volatile unsigned long *)start, *ee=(volatile unsigned long *)end;
    while(ss<ee){
        asm_movnti(ss, pattern);
        ss++;
    }
}
static inline void memsetblock(void *start, void *end, word pattern){
    memsetblocknodrain(start, end, pattern);
    sfence();
}
#define SYNC(prm) sync((volatile void *)(prm))
#define SYNCR(s,e) sync_range((volatile void*)(s), (volatile void *)(e))


static inline void write(word *addr, word val){
    //asm_movnti((volatile unsigned long*)addr, val);
    *addr = val;
}



#endif /* fences_h */
