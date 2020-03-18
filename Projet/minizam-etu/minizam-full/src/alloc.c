#include <stdlib.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>


// mlvalue* caml_alloc(size_t size) {
//   return aligned_alloc(8,size);
// }

mlvalue* caml_alloc(size_t size) {
  size = size / sizeof(mlvalue);
  uint64_t total = Heap_size/sizeof(mlvalue);
  if(Caml_state->alloc_ptr > total){
    printf("error alloc, maxsize=%ld\n", total);
    exit(0);
  }
  mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
  Caml_state->alloc_ptr += size;
  // printf("allocate=%ld, aptr=%ld, rest=%ld\n", size, Caml_state->alloc_ptr, total - Caml_state->alloc_ptr);
  return res;
}

