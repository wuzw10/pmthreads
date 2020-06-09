#ifndef COUPLING_H
#define COUPLING_H
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "linkedlist-lock.h"

int lockc_delete(intset_l_t *set, val_t val);
int lockc_find(intset_l_t *set, val_t val);
int lockc_insert(intset_l_t *set, val_t val);

void debug(intset_l_t *set);
    
#ifdef __cplusplus
}
#endif

#endif
