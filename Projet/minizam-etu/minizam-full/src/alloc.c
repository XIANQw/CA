#include <stdlib.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>
#include "gc.h"

#ifdef STOP_n_COPY

mlvalue* caml_alloc(size_t size) {
  int had_gc = 0;
  if (Caml_state->alloc_ptr + size > SIZE){
    gc();
    had_gc = 1;
    // printf("Gc: alloc_ptr=%ld, alloc_size=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, size, SIZE);
  }
  if(had_gc && Caml_state->alloc_ptr >= SIZE/2){
    SIZE = (size_t)(SIZE * 1.5);
    resize();
    // printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
  }
  else if(had_gc && Caml_state->alloc_ptr < SIZE/2){
    SIZE = (size_t)(SIZE/1.5);
    resize();
    // printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
  }
  mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
  Caml_state->alloc_ptr += size;
  return res;
}
#endif

#ifdef MARK_n_SWEEP


#endif