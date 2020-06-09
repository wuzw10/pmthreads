//
//  barrier.cpp
//  NUMA
//
//  Created by WUZHENWEI on 2019/6/19.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#include "barrier.hpp"


void barrier_init(barrier_t *b, int n)
{
    pthread_cond_init(&b->complete, NULL);
    pthread_mutex_init(&b->mutex, NULL);
    b->count = n;
    b->crossing = 0;
}


#include <sys/types.h>
#include <unistd.h>


void barrier_cross(barrier_t *b)
{
#ifndef NVTHREADS_TEST
    pthread_mutex_lock(&b->mutex);
    /* One more thread through */
    b->crossing++;
    /* If not all here, wait */
    if (b->crossing < b->count) {
        pthread_cond_wait(&b->complete, &b->mutex);
    } else {
        pthread_cond_broadcast(&b->complete);
        /* Reset for next time */
        b->crossing = 0;
    }
    pthread_mutex_unlock(&b->mutex);
#endif
}


void bind_to_cpu(int cpu)
{
#ifndef NVTHREADS_TEST
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    int s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    assert(s == 0);
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    int s= sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);
    assert(s == 0);
#endif
}

