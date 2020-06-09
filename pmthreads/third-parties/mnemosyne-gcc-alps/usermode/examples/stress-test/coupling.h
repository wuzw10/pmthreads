#ifndef COUPLING_H
#define COUPLING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "linkedlist-lock.h"
#include "pvar.h"
	
int TM_ATTR lockc_delete(intset_l_t *set, val_t val);
int TM_ATTR lockc_find(intset_l_t *set, val_t val);
int TM_ATTR lockc_insert(intset_l_t *set, val_t val);

    
#ifdef __cplusplus
}
#endif

#endif
