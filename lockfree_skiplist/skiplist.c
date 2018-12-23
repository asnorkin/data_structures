#include "skiplist.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include "marked_pointers.h"


#define err_exit(msg)   do{                         \
                            perror(msg);            \
                            exit(EXIT_FAILURE);     \
                        } while(0)


node_t *node_init(val_t value, node_t *next, node_t *down);


slist_t *sl_init() {
    slist_t *slist = (slist_t *)calloc(1, sizeof(slist_t));
    if(!slist)
        err_exit("Can't allocate memory for lock-free skiplist");

    for(int i = MAX_LEVEL - 1; i >= 0; --i)
        slist->levels[i] = lfl_init();

    for(int i = MAX_LEVEL - 1; i > 0; --i) {
        slist->levels[i]->head->down = slist->levels[i - 1]->head;
        slist->levels[i]->tail->down = slist->levels[i - 1]->tail;
    }

    //slist->level = 1;
    //slist->size  = 0;

    return slist;
}


void sl_fini(slist_t *slist) {
    for(int i = MAX_LEVEL - 1; i >= 0; --i)
        lfl_fini(slist->levels[i]);
}

//  It needs to check
val_t sl_find(slist_t *slist, key_t key) {
    node_t *start_node = slist->levels[MAX_LEVEL - 1]->head;
    node_t *left = (node_t *)calloc(1, sizeof(node_t));
    for(int i = MAX_LEVEL - 1; i >= 0; --i) {
        node_t *right = lfl_find_node(slist->levels[i], key, &left, start_node);
        if(right && right->key == key)
            return right->value;               

        start_node = left->down;
    }

    return -1;
}


int sl_add(slist_t *slist, key_t key, val_t val) {
    //  Go down level by level and find places to insert
    node_t *prev[MAX_LEVEL];
    node_t *start_node = slist->levels[MAX_LEVEL - 1]->head;
    for(int i = MAX_LEVEL - 1; i >= 0; --i) {
        node_t *right = lfl_find_node(slist->levels[i], key, &prev[i], start_node);
        if(right && right->key == key)
            return -1;

        start_node = prev[i]->down;
    }

    //  Add to each level from the bottom while
    unsigned int seed = time(NULL);
    for(int i = 0; i < MAX_LEVEL; ++i) {
        lfl_add(slist->levels[i], key, val, prev[i]);
        if(rand_r(&seed) > RAND_MAX / 2)
            break;
    }

    return 0;
}


void sl_print(slist_t *slist) {
    printf("\n############################\n");
    printf("###  lock-free skiplist  ###\n");
    printf("############################\n");
    //printf("### size : %u\n", slist->size);
    for(int i = MAX_LEVEL - 1; i >= 0; --i) {
        printf("level %d : ", i);
        print_level(slist, i);
        printf("\n");
    }

    printf("########################\n");
}

//  Doesn't work right
void print_level(slist_t *slist, int level) {
    lf_list_t *list = slist->levels[level];
    node_t *prev_node = NULL;
    if(level > 0)
        prev_node = slist->levels[level - 1]->head->next;

    node_t *curr_node = list->head->next;
    while(curr_node != list->tail) {
        if(is_marked_ref(curr_node)) {
            curr_node = get_unmarked_ref(curr_node);
            curr_node = curr_node->next;
            continue;
        }

        if(level > 0) {
            lf_list_t *prev_list = slist->levels[level - 1];
            node_t *prev_curr_node = prev_node->next;
            while(prev_curr_node != prev_list->tail &&
                  prev_curr_node->key != curr_node->key) {
                printf("      ");
                prev_node = prev_curr_node;
                prev_curr_node = prev_curr_node->next;
            }
        }

        printf("(%ld, %ld) ", curr_node->key, curr_node->value);
        curr_node = curr_node->next;
    }
}
