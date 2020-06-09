//
//  main.cpp
//  ubuntu
//
//  Created by WUZHENWEI on 2019/6/24.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//
//#include "posixlock.h"
#include "pvar.h"
#include <mtm.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_options.h"
#include "barrier.hpp"
#include <pmalloc.h>
#include <mnemosyne.h>
#include <mtm.h>
#include <pmalloc.h>
#include "btree.h"
#include <chrono>
#include "posixlock.h"
#include <random>

#define USE_CPU_1 10
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_args {
	int tid;
	barrier_t * barrier;
	options_t * options;
} thread_data;

void bind_to_cpu(int cpu)
{

	int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
	cpu = cpu % numCPU;
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu, &cpuset);
	int s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
	assert(s == 0);
}


entry_key_t*  keys;


std::default_random_engine generator;
std::uniform_int_distribution<entry_key_t> distribution(0, 1000000000000);
std::uniform_real_distribution<float> ratio_distribution(0, 1.0);


unsigned long global_ops = 0;

const int value_size = 256;

long* nvptr = NULL;

btree* bt = NULL;


#include "coupling.h"
intset_l_t * the_list;

#if 0
void* thr_func(void* args) {
	int r = 40;
	while (r--) {
		PTx
		{
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			*nvptr = *nvptr + 1;
			//global_ops++;
		}
		usleep(100);
	}
	pthread_mutex_lock(&mtx);
	pthread_mutex_unlock(&mtx);

	return NULL;
}
#else
thread_local bool delete_element = true;
void* thr_func(void* args) {

	thread_data* targs = (thread_data*)args;
	int tid = targs->tid;
	options_t* opts = targs->options;
	unsigned long ops = 0;
	unsigned long rop = 0;
	unsigned long wop = 0;


	int cpu_offset = 10;

	if(_options.num_threads > 10){
		cpu_offset = 0;
	}

	if (_options.bind_cpu) {

		bind_to_cpu(tid + cpu_offset); //use numa node 1 
	}


	int round = _options.tx_size;

	int local_operations = _options.total_operations / _options.num_threads;


	//#puts < half of the keys array
	assert(_options.total_operations * (1 - _options.read_ratio) <= _options.num_entries / 2);

	int local_put = (int)(local_operations * (1 - _options.read_ratio ));
	if (_options.verbose) fprintf(stderr, "local_put is %d\n", local_put);


	int pput = local_put * tid;
	if (_options.verbose) fprintf(stderr, "Tid %d pput is %d\n", tid, pput);


	barrier_cross(targs->barrier);

	//if(_options.verbose) fprintf(stderr, "Tid %d crossed barrier.\n", tid);

	auto start = std::chrono::steady_clock::now();

	while (local_operations > 0) {
		//PTx{
		//pthread_mutex_lock(&mtx);
		while (round-- && (local_operations > 0)) {
			local_operations--;

			float ratio = ratio_distribution(generator);
			if (ratio < _options.read_ratio) {
				entry_key_t search_key = distribution(generator) % _options.num_entries;
				
				if(_options.target == LIST){
					PTx{
						lockc_find(the_list, search_key);
					}
				}else{
					PTx{
						char * buf = bt->btree_search(search_key);
					}
				}
				rop ++;
			}
			else {
				unsigned index = _options.num_entries / 2 + pput;
				pput++;
				if (!(index < _options.num_entries)) {
					printf("Tid %d index is %d %lu pput %d\n", tid, index, _options.num_entries, pput);
				}
				assert(index < _options.num_entries);
				entry_key_t key = keys[index];
			
				if(_options.target == LIST){
					if(delete_element){
						entry_key_t remove_key = keys[distribution(generator) % _options.num_entries];
						PTx{
							lockc_delete(the_list, remove_key);
						}
					}else{
						PTx{
							lockc_insert(the_list, key);
						}
					}
					delete_element = !delete_element;
				}else{
					char* buf = (char*)pmalloc(value_size);
					sprintf(buf, "value of key%lu", key);
					PTx{
						bt->btree_insert(key, buf);
					}
				}
				wop ++;
			}
			ops++;
		}
		round = _options.tx_size;
		//}
	}

	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;

	pthread_mutex_lock(&mtx);
	printf("Thread %d performed %lu ops. \n", tid, ops);
	global_ops += ops;
	double ms = std::chrono::duration <double, std::milli> (diff).count();

	if (_options.verbose)
		printf("%d %lu (r %lu, w%lu) run %lf ms %lf ops/ms\n", tid, ops, rop, wop, ms, (double)ops / ms);

	pthread_mutex_unlock(&mtx);

	return NULL;
}
#endif


void load_input_data();

int main(int argc, char *argv[])
{
	bind_to_cpu(0);

	get_options(argc, argv);

	if(_options.target == BTREE){
		bt = new btree();
		fprintf(stderr, "\tEvaluation type: B+TREE\n");
	}else{
		fprintf(stderr, "\tEvaluation type: LIST\n");
		//printf("We are here.\n");
		the_list = set_new_l();
		//printf("We are here.\n");
	}
	// PTx{
	// 	bt = (btree*)pmalloc(sizeof(btree));//(btree*) pmalloc(sizeof(btree));
	// }

	printf("Hello\n");

	load_input_data();

	int num_threads = _options.num_threads;
	pthread_t* threads = new pthread_t[num_threads];

	thread_data * data = new thread_data[num_threads];

	barrier_t barrier;
	barrier_init(&barrier, num_threads + 1);

	for (int i = 0; i < num_threads; i++) {
		data[i].barrier = &barrier;
		data[i].tid = i;
		data[i].options = &_options;
		pthread_create(&threads[i], NULL, thr_func, &data[i]);
	}

	barrier_cross(&barrier);

	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < num_threads; i++) {
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

	return 0;
}


#define NUM_INIT_THREADS 1
void* init_func(void* args) {
	thread_data* targs = (thread_data*)args;
	int tid = targs->tid;

	int half_nkeys = _options.num_entries / 2;
	int keys_per_threads = half_nkeys / NUM_INIT_THREADS;

	unsigned from = tid * keys_per_threads;
	unsigned to = from + keys_per_threads;

	//pthread_mutex_lock(&mtx);
	for (int i = from; i < to; i++) {
		if(_options.target == LIST){
			PTx{	
				lockc_insert(the_list, keys[i]);
			}
			//printf("inster to list\n");
		}else{
			char* val = (char*)pmalloc(value_size);//new char[value_size];
			sprintf(val, "VAL::%lu", keys[i]);
			PTx{
				bt->btree_insert(keys[i], val);
			}
		}


	}
	//pthread_mutex_unlock(&mtx);

	return NULL;
}

void load_input_data() {
	int nkeys = _options.num_entries;
	assert(nkeys <= 2000000);

	keys = new entry_key_t[nkeys];

	int num_threads = NUM_INIT_THREADS;
	pthread_t* threads = new pthread_t[num_threads];
	thread_data * data = new thread_data[num_threads];

	std::ifstream ifs;
	ifs.open("/tmp/uniform_input.txt");
	if (!ifs) {
		std::cout << "input loading error!" << std::endl;
		exit(-1);
	}

	for (unsigned i = 0; i < nkeys; i++) {
		ifs >> keys[i];
	}


	for (int i = 0; i < num_threads; i++) {
		data[i].tid = i;
		pthread_create(&threads[i], NULL, init_func, &data[i]);
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	fprintf(stderr, "Finish warming up. \n");
}
