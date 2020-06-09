#ifndef __NVMALLOC__
#define __NVMALLOC__

#ifdef HOARD_NVMALLOC
extern void* nvmalloc(size_t sz, const char* label);
extern void nvfree(void* ptr);
#else
#define nvmalloc(sz, label) malloc(sz)
#define nvfree(ptr) free(ptr)
#endif

#endif
