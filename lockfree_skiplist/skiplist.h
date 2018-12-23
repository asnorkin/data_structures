#ifndef LF_SKIPLIST_H
#define LF_SKIPLIST_H


#include "lf_list.h"


#define MAX_LEVEL   16


typedef struct skiplist {
    //unsigned level;
    //unsigned size;
    lf_list_t *levels[MAX_LEVEL];
} slist_t;


slist_t *sl_init();
void    sl_fini(slist_t *slist);
val_t   sl_find(slist_t *slist, key_t key);
int     sl_add(slist_t *slist, key_t key, val_t val);

void sl_print(slist_t *slist);

node_t *sl_find_node(slist_t *slist, key_t key);


#endif // LF_SKIPLIST_H
