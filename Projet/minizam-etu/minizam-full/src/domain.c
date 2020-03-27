#include <stdlib.h>

#include "domain_state.h"
#include "config.h"
#include "mlvalues.h"


caml_domain_state* Caml_state;

void caml_init_domain() {

  Caml_state = malloc(sizeof(caml_domain_state));
  Caml_state->stack = malloc(Stack_size);
  Caml_state->sp = 0;

  
  #ifdef STOP_n_COPY
  Caml_state->heap_size = (size_t)(Heap_size/sizeof(mlvalue));
  Caml_state->heap_a = malloc(Heap_size);
  Caml_state->heap_b = malloc(Heap_size);
  Caml_state->alloc_ptr = 0;
  #endif
} 
