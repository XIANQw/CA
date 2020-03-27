#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "gc.h"

typedef struct _caml_domain_state {
  /* Stack */
  mlvalue* stack;
  unsigned int sp;
  mlvalue env;
  
  #ifdef STOP_n_COPY
  mlvalue* heap_a;
  mlvalue* heap_b;
  uint64_t alloc_ptr;
  size_t heap_size;
  #endif
} caml_domain_state;

/* The global state */
extern caml_domain_state* Caml_state;

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
