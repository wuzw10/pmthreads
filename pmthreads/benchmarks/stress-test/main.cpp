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
#include "posixlock.h"
#include "get_options.h"
#include "barrier.hpp"
#include "concurrent_btree.hpp"
#include <random>
#include <atomic>
#include <chrono>
//#include <list>
#include <string>
#include <iostream>
#include <fstream>

#ifdef ATLAS_TEST
#include "atlas_api.h"
#endif


#include "coupling.h"
intset_l_t * the_list;


typedef HL::PosixLockType lock_t;

unsigned long num_persist = 0;
void persist(){
	num_persist++;
	printf("persist %lu times ...\n", num_persist);
}

lock_t locks[4];


pthread_mutex_t mtx;
//= PTHREAD_MUTEX_INITIALIZER;

typedef concurrent::btree btree_t;
btree_t btree;

typedef struct thread_args{
	int tid;
	barrier_t * barrier;
	options_t * options;
}thread_data;

std::default_random_engine generator;
std::uniform_int_distribution<entry_key_t> distribution(0, 1000000000000);

std::uniform_real_distribution<float> ratio_distribution(0, 1.0);

std::atomic<bool> __stop__;

unsigned long global_ops = 0;

const int value_size = 256;


#define MAX_THREADS 64
HL::PosixLockType __mtx__[MAX_THREADS];
#define TX_BEGIN(tid) do{\
	__mtx__[tid].lock();\
}while(0)
#define TX_END(tid) do{\
	__mtx__[tid].unlock();\
}while(0)

//#define USE_CPU_1 10

entry_key_t*  keys;

thread_local bool delete_element = true;

void* thr_func(void* args){

	thread_data* targs = (thread_data*)args;
	int tid = targs->tid;
	options_t* opts = targs->options;
	unsigned long ops = 0;
	unsigned long rop = 0;
	unsigned long wop = 0;

	//When total threads below 10, use CPU 1
	int cpu_offset = 0;
	if (_options.num_threads <= 10){
		cpu_offset = 10;
	}

	if(_options.bind_cpu){
		const int MAX_AVAILABLE_CORES  = sysconf(_SC_NPROCESSORS_ONLN);
		int bind_to = (tid) % MAX_AVAILABLE_CORES;
		bind_to_cpu(bind_to); 
	}

	int round = _options.tx_size;

	int local_operations = _options.total_operations / _options.num_threads;


	//#puts < half of the keys array
	assert(_options.total_operations * (1- _options.read_ratio) <= _options.num_entries/2);

	int local_put = (int)(local_operations *(1 - _options.read_ratio ));
	if(_options.verbose) fprintf(stderr, "local_put is %d\n", local_put);


	int pput = local_put * tid;
	if(_options.verbose) fprintf(stderr, "Tid %d pput is %d\n", tid, pput);


	barrier_cross(targs->barrier);

	if(_options.verbose) fprintf(stderr, "Tid %d crossed barrier.\n", tid);

	auto start = std::chrono::steady_clock::now();

	while(local_operations > 0){
		//while(__stop__.load(std::memory_order_relaxed) == false){

		//TX_BEGIN(tid);

		while(round-- && (local_operations > 0)){
			local_operations--;

			float ratio = ratio_distribution(generator);
			if(ratio < _options.read_ratio){
				entry_key_t search_key = keys[distribution(generator) % _options.num_entries];

				if(_options.target == LIST){
					lockc_find(the_list, search_key);
				}
				else{
					char * buf = btree.btree_search(search_key);
				}
				rop ++;
			}
			else{

				if(_options.target == LIST){
					if(delete_element){

						entry_key_t remove_key = keys[distribution(generator) % _options.num_entries];
						lockc_delete(the_list, remove_key);
					}else{
						unsigned index = _options.num_entries/2 + pput;
						pput++;
						if(!(index < _options.num_entries)){
							printf("Tid %d index is %d %lu pput %d\n", tid, index, _options.num_entries, pput);
						}
						assert(index < _options.num_entries);
						entry_key_t key = keys[index];



						lockc_insert(the_list, key);
					}

					delete_element = !delete_element;
				}else{

					unsigned index = _options.num_entries/2 + pput;
					pput++;
					if(!(index < _options.num_entries)){
						printf("Tid %d index is %d %lu pput %d\n", tid, index, _options.num_entries, pput);
					}
					assert(index < _options.num_entries);
					entry_key_t key = keys[index];

					//entry_key_t remove_key = keys[distribution(generator) % _options.num_entries];
					char* buf = new char[256];
					sprintf(buf, "value of key%lu", key);
					btree.btree_insert(key, buf);

				}
				wop ++;
			}
			ops++;
		}
		round = _options.tx_size;

		//TX_END(tid);

	}
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;

	pthread_mutex_lock(&mtx);
	//printf("Thread %d performed %lu ops. \n", tid, ops);
	global_ops += ops;
	double ms = std::chrono::duration <double, std::milli> (diff).count();

	if(_options.verbose)
		printf("%d %lu (r %lu, w%lu) run %lf ms %lf ops/ms\n", tid, ops, rop, wop, ms, (double)ops/ms);

	pthread_mutex_unlock(&mtx);

	return NULL;
	}


	void load_input_data();


	int main(int argc, char *argv[])
	{
		bind_to_cpu(0);


#ifdef ATLAS_TEST
		NVM_Initialize();
#endif

		the_list = set_new_l();


		pthread_mutex_init(&mtx, NULL);



		//locks = new lock_t[4];
		get_options(argc, argv);



		load_input_data();


		int num_threads = _options.num_threads;
		pthread_t* threads = new pthread_t[num_threads];

		thread_data * data = new thread_data[num_threads];

		barrier_t barrier;
		barrier_init(&barrier, num_threads + 1);

		for (int i=0; i<num_threads; i++) {
			data[i].barrier = &barrier;
			data[i].tid = i;
			data[i].options = &_options;
			pthread_create(&threads[i], NULL, thr_func, &data[i]);
		}

		//__stop__.store(false, std::memory_order_release);
		barrier_cross(&barrier);
		fprintf(stderr, "MAIN THREAD CROSSED BARRIER. \n");

		auto start = std::chrono::steady_clock::now();





		//    int xxx = 10;
		//    while(xxx --){
		//        TX_BEGIN(32);
		//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		//        TX_END(32);
		//    }






		//std::this_thread::sleep_for(std::chrono::milliseconds(_options.duration));

		//__stop__.store(true, std::memory_order_release);

		for (int i=0; i<num_threads; i++) {
			pthread_join(threads[i], NULL);
		}

		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;

		double duration_in_ms = std::chrono::duration <double, std::milli> (diff).count();

		double throughput = (double)global_ops / (double) duration_in_ms;

		fprintf(stderr, "Aggregated Throughput: %lf ops/ms\n", throughput);

		delete []data;
		delete []threads;
		delete []keys;

#ifdef ATLAS_TEST
		NVM_Finalize();
#endif



		return 0;
	}

#define NUM_INIT_THREADS 10

	void* init_func(void* args){
		thread_data* targs = (thread_data*)args;
		int tid = targs->tid;

		int half_nkeys = _options.num_entries/2;
		int keys_per_threads = half_nkeys/NUM_INIT_THREADS;

		unsigned from = tid*keys_per_threads;
		unsigned to = from + keys_per_threads;

		const int MAX_AVAILABLE_CORES  = sysconf(_SC_NPROCESSORS_ONLN);
		bind_to_cpu(tid%MAX_AVAILABLE_CORES);

		TX_BEGIN(tid);

		for (int i=from; i<to; i++) {
			if(_options.target == LIST){
				lockc_insert(the_list, keys[i]);
			}else{
				char* val = new char[value_size];
				sprintf(val, "VAL::%lu", keys[i]);
				btree.btree_insert(keys[i], val);
			}
		}

		TX_END(tid);

		return NULL;
	}

	void load_input_data(){
		int nkeys = _options.num_entries;
		assert(nkeys <= 2000000);
		keys = new entry_key_t[nkeys];

		int num_threads = NUM_INIT_THREADS;
		pthread_t* threads = new pthread_t[num_threads];
		thread_data * data = new thread_data[num_threads];

		std::ifstream ifs;
		ifs.open("./inputs/uniform_input.txt");
		if(!ifs){
			std::cout << "input loading error!" << std::endl;
			exit(-1);
		}

		for(unsigned i=0; i<nkeys; i++){
			ifs >> keys[i]; 
		}


		for (int i=0; i<num_threads; i++) {
			data[i].tid = i;
			pthread_create(&threads[i], NULL, init_func, &data[i]);
		}

		for (int i=0; i<num_threads; i++) {
			pthread_join(threads[i], NULL);
		}

		fprintf(stderr, "Finish warming up. \n");
	}
