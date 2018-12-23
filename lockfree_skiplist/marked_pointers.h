#ifndef MARKED_POINTERS_H
#define MARKED_POINTERS_H

/*
 *  This funcs are for logical deletion of pointers
 *  If first bit of pointer is 1 => node was logically deleted
 */

int     is_marked_ref(long i);
long    unset_mark(long i);
long    set_mark(long i);
long    get_unmarked_ref(long w);
long    get_marked_ref(long w);

#endif // MARKED_POINTERS_H
