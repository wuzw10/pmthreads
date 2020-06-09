//
//  barrier.hpp
//  NUMA
//
//  Created by WUZHENWEI on 2019/6/19.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#ifndef barrier_hpp
#define barrier_hpp
#include <pthread.h>
#include <sched.h>
#include <assert.h>

void bind_to_cpu(int cpu);

typedef struct barrier {
    pthread_cond_t complete;
    pthread_mutex_t mutex;
    int count;
    int crossing;
} barrier_t;

void barrier_init(barrier_t *b, int n);
void barrier_cross(barrier_t *b);

#endif /* barrier_hpp */
