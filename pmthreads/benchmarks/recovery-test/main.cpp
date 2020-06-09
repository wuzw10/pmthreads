//
//  main.cpp
//  ubuntu
//
//  Created by WUZHENWEI on 2019/6/24.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_options.h"
#include "barrier.hpp"
#include <random>
#include <atomic>
#include <chrono>
#include <list>
#include <thread>
#include <sys/mman.h>
#include <new>
#include "coupling.h"

#include "nvrecover.h"
#include "concurrent_btree.hpp"


#ifdef ATLAS_TEST
#include "atlas_api.h"
#endif

#define BEGIN() pthread_mutex_lock(&mtx)
#define END() pthread_mutex_unlock(&mtx)
pthread_mutex_t mtx;

typedef int64_t entry_key_t;


typedef struct thread_args{
    int tid;
    barrier_t * barrier;
    options_t * options;
}thread_data;

std::default_random_engine generator;
std::uniform_int_distribution<unsigned long> distribution(0, 1000000000000);
std::uniform_int_distribution<unsigned long> distribution2(0, 100);
std::uniform_real_distribution<float> ratio_distribution(0, 1.0);
std::atomic<bool> __stop__;

barrier_t barrier;

entry_key_t*  keys = NULL;

typedef concurrent::btree btree_t;
btree_t* btree = NULL;

intset_l_t* ll = NULL;

void* thr_func(void* args){
    thread_data* targs = (thread_data*)args;
    int tid = targs->tid;
    options_t* opts = targs->options;
	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	bind_to_cpu(tid % numCPU);
    
    barrier_cross(targs->barrier);
    
    unsigned long num = _options.num_entries;
    unsigned long len = num / _options.num_threads;
    
    unsigned long start = tid*len;
    unsigned long end = start + len;
    
    if(!is_crashed()){
        printf("T%d not crashed, will insert %lu elements. \n", tid, len);
        unsigned long i = start;
        while(!__stop__.load(std::memory_order_relaxed)){

            if(_options.selection)
            {
                btree->btree_insert(keys[i], (char*)((void*)(keys[i])));
            }
            else{
                lockc_insert(ll, keys[i]);    
            }
            
            i++;
            if(i==end)
                break;
        }
    }  
    printf("T%d exit.\n", tid);
    
    return NULL;
}


void load_input_data();

int main(int argc, char *argv[])
{
    
    #ifdef ATLAS_TEST
        NVM_Initialize();
    #endif

    get_options(argc, argv);

    load_input_data();
    
    pthread_mutex_init(&mtx, NULL);
    int num_threads = _options.num_threads;
    pthread_t* threads = new pthread_t[num_threads];
    thread_data * data = new thread_data[num_threads];
    barrier_init(&barrier, num_threads + 1); //worker_threads + main_thread
    
    for (int i=0; i<num_threads; i++) {
        data[i].barrier = &barrier;
        data[i].tid = i;
        data[i].options = &_options;
        pthread_create(&threads[i], NULL, thr_func, &data[i]);
    }
    
    __stop__.store(false, std::memory_order_release);
    /******************************************************/
    
    
    if(!is_crashed()){
        BEGIN();
        if(_options.selection){
            char* buf = (char*)nvmalloc(sizeof(btree_t), "xxx");
            btree = new (buf) btree_t();// (buf);//nvmalloc_root(sizeof(btree_t), "root");
            install_pm_handle("root", (void*)(btree));
            printf("[DEBUG] addr btree: %p\n", (void*)btree);
            
        }
        else{
            node_l_t *min, *max;
            ll = (intset_l_t*)nvmalloc_root(sizeof(intset_l_t), "set");    
            printf("addr ll: %p\n", (void*)ll);
            //initialization
            max = (node_l_t *)nvmalloc(sizeof(node_l_t), "max");
            max->val = VAL_MAX;
            max->next = NULL;
            INIT_LOCK(&(max->lock));
            min = (node_l_t *)nvmalloc(sizeof(node_l_t), "min");
            min->val = VAL_MIN;
            min->next = max;
            INIT_LOCK(&(min->lock));
            ll->head = min;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        END();
    }
    else{
        BEGIN();
        if(_options.selection){
            btree = (btree_t*) pm_recover("root");
            printf("[DEBUG] addr btree: %p\n", (void*)btree);
        }
        else{
            ll = (intset_l_t*)pm_recover("set");
            printf("addr ll: %p\n", (void*)ll);
        }
        END();
    }
    /******************************************************/
    barrier_cross(&barrier);
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(_options.duration));
    
    
    
    //Timed Abort
    
    if(!is_crashed()){
        //quit ative list
        //sleep
        //rejoin active list
        pmthreads_sleep(_options.duration); //Enable commit
        
        __stop__.store(true, std::memory_order_release);
        
        for (int i=0; i<num_threads; i++) {
            pthread_join(threads[i], NULL);
        }
        
        if(_options.selection){
            size_t sz = btree->size();//size(btree);
            fprintf(stderr, "#successfully inserted DRAM elements: %ld %d\n", sz, btree->height);
        }
        else{
            size_t sz = set_size_l(ll);
            fprintf(stderr, "#successfully inserted DRAM elements: %ld\n", sz);
        }
        
        //sleep random (0, 100) ms
        unsigned long tt = distribution2(generator);
        assert(tt>=0 && tt<=100);
        pmthreads_sleep(tt);
        
        
        
        abort();
    }
    
    
    for (int i=0; i<num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    

    if(_options.selection){
        size_t sz = btree->size();
        printf("#successfully recovered keys: %ld\n", sz);
    }
    else{
        int sz = set_size_l(ll);
        printf("#successfully recovered keys: %d\n", sz);
    }
    


    
    
    printf("\n");
    printf("Main exit.\n");
    
    delete []data;
    delete []threads;


    #ifdef ATLAS_TEST
    NVM_Finalize(); 
    #endif



    return 0;
}


void load_input_data(){
    int nkeys = _options.num_entries;
    assert(nkeys <= 2000000);
    keys = new entry_key_t[nkeys];
    std::ifstream ifs;
    ifs.open("./uniform_input.txt");
    if(!ifs){
        std::cout << "input loading error!" << std::endl;
        exit(-1);
    }
    for(unsigned i=0; i<nkeys; i++){
        ifs >> keys[i];
    }
    fprintf(stderr, "Finish warming up. \n");
}

