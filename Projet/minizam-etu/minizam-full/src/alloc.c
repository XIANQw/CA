#include <stdlib.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>
#include "gc.h"

size_t next_size = (size_t)(Heap_size/sizeof(mlvalue));

mlvalue* caml_alloc(size_t size) {
  int had_gc = 0;
  while (Caml_state->alloc_ptr + size > SIZE){
    // printf("Heap full, size=%ld, next_size=%ld\n", SIZE, next_size);
    SIZE = next_size;
    gc(SIZE);
    had_gc = 1;
    // printf("Gc: alloc_ptr=%ld, size=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, size, SIZE);
    if(Caml_state->alloc_ptr + size > SIZE) next_size = (size_t)(SIZE*1.5);
  }
  if(had_gc && Caml_state->alloc_ptr >= SIZE/2){
    // printf("alloc_ptr=%ld>=%ld\n", Caml_state->alloc_ptr, SIZE/2);
    // next_size = (size_t)(SIZE*1.5);
  }
  else if(had_gc && Caml_state->alloc_ptr < SIZE/2){
    // printf("alloc_ptr=%ld<%ld\n", Caml_state->alloc_ptr, SIZE/2);
    // next_size = (size_t)(SIZE/1.5);
  }
  mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
  Caml_state->alloc_ptr += size;
  return res;
}

