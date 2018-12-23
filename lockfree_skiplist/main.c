#include <stdio.h>

#include "skiplist.h"
#include "lf_list.h"

int main(void)
{
    /*lf_list_t *list = lfl_init();
    lfl_add(list, 5, 5, NULL);
    lfl_print(list);
    lfl_add(list, 4, 4, NULL);
    lfl_print(list);
    lfl_add(list, 6, 6, NULL);
    lfl_print(list);
    lfl_add(list, 6, 7, NULL);
    lfl_print(list);
    lfl_del(list, 4);
    lfl_print(list);
    lfl_del(list, 5);
    lfl_print(list);
    lfl_del(list, 6);
    lfl_print(list);*/

    slist_t *slist = sl_init();
    sl_add(slist, 3, 3);
    sl_print(slist);
    sl_add(slist, 2, 2);
    sl_print(slist);
    sl_add(slist, 4, 4);
    sl_print(slist);
    sl_add(slist, 6, 6);
    sl_print(slist);
    sl_add(slist, 5, 5);
    sl_print(slist);

    return 0;
}

