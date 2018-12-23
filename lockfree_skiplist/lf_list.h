#ifndef LF_LIST_H
#define LF_LIST_H


typedef long val_t;
typedef long key_t;


typedef struct node {
    key_t key;
    val_t value;
    struct lfl_node *next;
    struct lfl_node *down;  //  Only for skiplist
} node_t;


typedef struct lf_list {
    struct node *head;
    struct node *tail;
    unsigned size;
} lf_list_t;


lf_list_t *lfl_init();
void lfl_fini(lf_list_t *list);
void lfl_print(lf_list_t *list);
val_t lfl_find(lf_list_t *list, key_t key, node_t **left);
int lfl_del(lf_list_t *list, key_t key);

//  Adding new element to the list
//
//  ARGUMENTS
//  ---------
//  list:   linked list
//  key:    key of new element
//  value:  new value
//
//  RETURNED VALUE
//  --------------
//  0 if success or -1
//  It may fails if such key has already exist in the list
//
int lfl_add(lf_list_t *list, key_t key, val_t value, node_t *start);

//  Finds the node by key from the start node.
//
//  ARGUMENTS
//  ---------
//  list:   linked list
//  key:    key to find
//  left:   pointer to a pointer to save element before returned
//  start:  pointer to a node to start. If it is NULL then start from head
//
//  RETURNED VALUE
//  --------------
//  Pointer to a node with the key if it exists
//  or pointer to a node with key next to required
//  If start node key is greater than required key => returns NULL
//
node_t *lfl_find_node(lf_list_t *list, key_t key, node_t **left, node_t *start);


#endif // LF_LIST_H
