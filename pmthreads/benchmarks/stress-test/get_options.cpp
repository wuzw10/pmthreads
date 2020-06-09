//
//  get_options.c
//  ubuntu
//
//  Created by WUZHENWEI on 2019/6/24.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#include "get_options.h"
#include "concurrent_btree.hpp"

options_t _options;

#define DEFAULT_NUM_THREADS 4
#define DEFAULT_NUM_ROUNDS  1000

#define OPTSTR "t:r:b:d:n:v:s:p:e:"
void get_options(int argc, char *argv[]){
	_options.num_threads = DEFAULT_NUM_THREADS;
	_options.read_ratio = 0.5;
	_options.bind_cpu = true;
	_options.duration = 2000;
	_options.num_entries = 2000000;
	_options.verbose = false;
	_options.tx_size = 1;
	_options.total_operations = 1000000;
	_options.eval_type = 0;
	_options.target = BTREE;
	int opt;
	while ((opt = getopt(argc, argv, OPTSTR)) != EOF){
		switch(opt) {
			case 't':
				_options.num_threads = atoi(optarg);
				break;
			case 'r':
				_options.read_ratio = atof(optarg);
				break;
			case 'b':
				_options.bind_cpu = atoi(optarg);
				break;
			case 'd':
				_options.duration = atol(optarg);
				break;
			case 'n':
				_options.num_entries = atol(optarg);
				break;
			case 'v':
				_options.verbose = atoi(optarg);
				break;
			case 's':
				_options.tx_size = atoi(optarg);
				break;
			case 'p':
				_options.total_operations = atol(optarg);
				break;
			case 'e':
				_options.eval_type = atoi(optarg);
				break;
			default:
				break;
		}
	}
	printf("Configurations:\n\tNum Threads: %d\n\tRead Ratio: %lf\n\tBind CPU: %d\n", _options.num_threads, _options.read_ratio, _options.bind_cpu);
	if(_options.eval_type)
	{
		_options.target=BTREE;
		printf("\tEvaluation type: B+TREE\n");
	}
	else
	{
		_options.target = LIST;
		printf("\tEvaluation type: LIST\n");
	}
	printf("\tNum Entries: %lu\n", _options.num_entries);
	printf("\tTotal Operations: %lu\n", _options.total_operations);
}
