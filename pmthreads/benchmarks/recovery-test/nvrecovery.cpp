#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#ifndef NVRECOVERY
unsigned long nvrecover(void *dest, size_t size, char *name){
	return 0;
}

void* nvmalloc(size_t sz, char* label){
	return malloc(sz);
}

void nvfree(void* ptr){
	free(ptr);
}

bool isCrashed() {return false;}
#endif

#ifdef NVTHREADS
void nvfree(void* ptr){
	free(ptr);
}
#endif

#ifdef __cplusplus
}
#endif
    
