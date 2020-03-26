#include <stdlib.h>

#include "alloc.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>
#include "gc.h"



mlvalue* caml_alloc(size_t size) {
  size = size / sizeof(mlvalue);
  uint64_t total = Caml_state->heap_size/sizeof(mlvalue); 
  if(Caml_state->alloc_ptr + size > total){ 
    printf("Heap full, gc start\n");
    gc(Caml_state->sp, Caml_state->stack);
    printf("Gc finish, alloc_ptr=%ld\n", Caml_state->alloc_ptr);
    if(Caml_state->alloc_ptr + size > total){
      printf("After gc, alloc_ptr= %ld ,heap full\n", Caml_state->alloc_ptr);
      exit(0);
    }
  }
  mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
  Caml_state->alloc_ptr += size;
  // printf("allocate=%ld, aptr=%ld, rest=%ld\n", size, Caml_state->alloc_ptr, total - Caml_state->alloc_ptr);
  return res;
}

