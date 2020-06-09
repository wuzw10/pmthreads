//
//  get_options.h
//  ubuntu
//
//  Created by WUZHENWEI on 2019/6/24.
//  Copyright © 2019年 WUZHENWEI. All rights reserved.
//

#ifndef get_options_h
#define get_options_h

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

enum TARGET{BTREE=0, LIST};
typedef struct options_structure{
    int num_threads;
    double read_ratio;
    bool bind_cpu;
    unsigned long duration;
    unsigned long num_entries;
    bool verbose;
    int tx_size;
    unsigned long total_operations;
	TARGET target;
    //int rounds;
    //PUT OPTIONS HERE 
}options_t;



extern options_t _options;

void get_options(int argc, char *argv[]);
#endif /* get_options_h */
