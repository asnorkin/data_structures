#include "marked_pointers.h"


int is_marked_ref(long i) {
  return (int) (i & 0x1L);
}


long unset_mark(long i) {
  i &= ~0x1L;
  return i;
}


long set_mark(long i) {
  i |= 0x1L;
  return i;
}


long get_unmarked_ref(long w) {
  return w & ~0x1L;
}


long get_marked_ref(long w) {
  return w | 0x1L;
}
