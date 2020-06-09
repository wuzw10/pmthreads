#ifndef NVRECOVER____
#define NVRECOVER____ 
#include <stdbool.h>
#include <stdlib.h>
#include <thread>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef PMTHREADS
extern void* pm_recover(char* handler);

extern void* pmalloc(size_t sz);
extern void nvfree(void *);
extern bool isCrashed();
    bool inline is_crashed(){
        return isCrashed();
    }

void* nvmalloc_root(size_t sz, char* handler);
    
inline void* nvmalloc(size_t sz, char* handler){
    (void)(handler);
    return pmalloc(sz);
}
extern void pmthreads_sleep (unsigned long ms);   
extern void install_pm_handle(const char* handle, void* ptr);
#endif
    
    
#ifdef ATLAS_TEST 
inline bool is_crashed(){ return false;}


inline void*  nvmalloc_root(size_t sz, char* handler){
    (void)(handler);
    return malloc(sz);
}
inline void* nvmalloc(size_t sz, char* handler){
    (void)(handler);
    return malloc(sz);
}
inline void install_pm_handle(const char* handle, void* ptr){}
inline void*  pm_recover(char* handle) {return NULL;}

inline void pmthreads_sleep (unsigned long ms){
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void nvfree(void* ptr){
    free(ptr);
}

#endif 

    
#ifdef __cplusplus
}
#endif

#endif
