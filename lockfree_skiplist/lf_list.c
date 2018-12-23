#include "lf_list.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "marked_pointers.h"


#define err_exit(msg)   do {                        \
                            perror(msg);            \
                            exit(EXIT_FAILURE);     \
                        } while(0)


node_t *node_init(key_t key, val_t val, node_t *next) {
    node_t *node = (node_t *)calloc(1, sizeof(node_t));
    if(!node)
        err_exit("Can't allocate memory for lock-free list new node");

    node->next  = next;
    node->key   = key;
    node->value = val;    

    return node;
}


lf_list_t *lfl_init() {
    lf_list_t *list = (lf_list_t *)calloc(1, sizeof(lf_list_t));
    if(!list)
        err_exit("Can't allocate memory for lock-free list");

    list->tail = node_init(INT_MAX, INT_MAX, NULL);
    list->head = node_init(INT_MIN, INT_MIN, list->tail);
    list->size = 0;

    return list;
}


void lfl_fini(lf_list_t *list) {
    node_t *curr_node = list->head->next;
    while(curr_node != list->tail) {
        lfl_del(list, curr_node->value);
        curr_node = list->head->next;
    }
}


void lfl_print(lf_list_t *list) {
    printf("\n########################\n");
    printf("###  lock-free list  ###\n");
    printf("########################\n");
    printf("### size : %u\n", list->size);

    //  Only for debug
    //printf("### head : %u, %ld\n", list->head, list->head->value);
    //printf("### tail : %u, %ld\n", list->tail, list->tail->value);
    node_t *curr_node = list->head->next;
    if(!curr_node)
        printf("# empty\n");

    while(curr_node != list->tail) {
        if(is_marked_ref(curr_node)) {
            curr_node = (node_t *)get_unmarked_ref(curr_node);
            curr_node = curr_node->next;
            continue;
        }

        printf("# %ld : %ld\n", curr_node->key, curr_node->value);
        curr_node = curr_node->next;
    }
    printf("########################\n");
}


node_t *lfl_find_node(lf_list_t *list, key_t key, node_t **left, node_t *start) {
    node_t *left_next, *right;
    left_next = right = NULL;
    if(!start)
        start = list->head;
    else if(start->key > key)
        return NULL;

    while(1) {
        node_t *curr = start;
        node_t *curr_next = start->next;

        //  Find place for new node
        while(is_marked_ref(curr_next) || (curr->key < key)) {
            //  Check node for logical deletion
            if(!is_marked_ref(curr_next)) {
                (*left) = curr;
                left_next = curr_next;
            }
            curr = get_unmarked_ref(curr_next);
            if(curr == list->tail)
                break;

            curr_next = curr->next;
        }
        right = curr;

        if(left_next == right) {
            if(!is_marked_ref(right->next))
                return right;
        } else {
            if(__sync_bool_compare_and_swap(&((*left)->next), left_next, right))
                return right;
        }
    }
}


val_t lfl_find(lf_list_t *list, key_t key, node_t **left) {
    node_t *left_next, *right;
    left_next = right = NULL;

    while(1) {
        node_t *curr = list->head;
        node_t *curr_next = list->head->next;

        //  Find place for new node
        while(is_marked_ref(curr_next) || (curr->key < key)) {
            //  Check node for logical deletion
            if(!is_marked_ref(curr_next)) {
                (*left) = curr;
                left_next = curr_next;
            }
            curr = get_unmarked_ref(curr_next);
            if(curr == list->tail)
                break;

            curr_next = curr->next;
        }
        right = curr;

        if(left_next == right) {
            if(!is_marked_ref(right->next))
                return right->value;
        } else {
            if(__sync_bool_compare_and_swap(&((*left)->next), left_next, right))
                return right->value;
        }
    }
}


int lfl_add(lf_list_t *list, key_t key, val_t value, node_t *start) {
    if(start && start->key >= key)
        return -1;

    node_t *left, *right;
    left = right = NULL;
    node_t *node = node_init(key, value, NULL);

    while(1) {
        right = lfl_find_node(list, key, &left, start);
        if(right && right->key == key)
            return -1;

        node->next = right;
        if(__sync_bool_compare_and_swap(&(left->next), right, node)) {
            __sync_fetch_and_add(&(list->size), 1);
            return 0;
        }
    }
}


int lfl_del(lf_list_t *list, key_t key) {
    node_t *right, *left, *left_next;
    right = left = left_next = NULL;

    while(1) {
        //  Check if our node exist
        right = lfl_find_node(list, key, &left, NULL);
        if(right == list->tail || right->key != key)
            return -1;

        left_next = left->next;
        if(!is_marked_ref(left_next)) {
            if(__sync_bool_compare_and_swap(&(left->next), left_next, get_marked_ref(left_next))) {
                __sync_fetch_and_sub(&(list->size), 1);
                return 0;
            }
        }
    }
}
