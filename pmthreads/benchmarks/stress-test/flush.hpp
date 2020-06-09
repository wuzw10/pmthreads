//
//  flsuh.hpp
//  NUMA
//
//  Created by WUZHENWEI on 2019/6/20.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#ifndef flsuh_hpp
#define flsuh_hpp

#include <stdio.h>
#include <stdint.h>

#define CACHE_LINE_SIZE 64

#define IS_FORWARD(c) (c % 2 == 0)

void inline clflush(char *data, int len){
//persistence-related operations in original FAST_FAIR are commented out
}


#ifdef HOARD_NVMALLOC

extern "C"{
    void* nvmalloc(size_t sz, const char* label);
    void nvfree(void* ptr);
}
#endif


#endif /* flsuh_hpp */
