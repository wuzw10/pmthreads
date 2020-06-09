/*
 * File:
 *   linkedlist-lock.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Lock-based linked list implementation of an integer set
 *
 * Copyright (c) 2009-2010.
 *
 * linkedlist-lock.c is part of Synchrobench
 *
 * Synchrobench is pfree software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "linkedlist-lock.h"

#include <stdio.h>
#include <pmalloc.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#ifdef __cplusplus
}
#endif


node_l_t* TM_ATTR new_node_l(val_t val, node_l_t *next, int transactional)
{
    node_l_t *node_l;
    node_l = (node_l_t *)pmalloc(sizeof(node_l_t));
    if (node_l == NULL) {
        //perror("pmalloc");
        //exit(1);
    }
    node_l->val = val;
    node_l->next = next;
    

    INIT_LOCK(&(node_l->lock));
    return node_l;
}

intset_l_t *set_new_l()
{
    
    intset_l_t *set;
    node_l_t *min, *max;
    
    void* ret = pmalloc(sizeof(intset_l_t));
    
    set = (intset_l_t *) ret;

    if ((set == NULL)) {
        //perror("pmalloc");
        //exit(1);
    }

    max = new_node_l(VAL_MAX, NULL, 0);
    min = new_node_l(VAL_MIN, max, 0);
    //printf("We are here error.\n");
    set->head = min;
    //printf("We are here correct.\n");
    return set;
}

void TM_ATTR node_delete_l(node_l_t *node) {
    DESTROY_LOCK(&node->lock);
    pfree(node);
}

void set_delete_l(intset_l_t *set)
{
    node_l_t *node, *next;
    
    node = set->head;
    while (node != NULL) {
        next = node->next;
        DESTROY_LOCK(&node->lock);
        pfree(node);
        node = next;
    }
    pfree(set);
}

int set_size_l(intset_l_t *set)
{
    int size = 0;
    node_l_t *node;
    
    /* We have at least 2 elements */
    node = set->head->next;
    while (node->next != NULL) {
        size++;
        node = node->next;
    }
    return size;
}
