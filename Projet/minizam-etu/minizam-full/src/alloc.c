#include <stdlib.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>

mlvalue* caml_alloc(size_t size) {
  if(Caml_state->alloc_ptr + size >= Heap_size){
    printf("error alloc\n");
    exit(0);
  }
  mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
  // printf("allocate %ld, rest %d\n", size, Heap_size - Caml_state->alloc_ptr);
  Caml_state->alloc_ptr += size;
  return res;
}

