#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "gc.h"
#include "alloc.h"


typedef struct _caml_domain_state {
  /* Stack */
    mlvalue* stack;
    unsigned int sp;
    mlvalue env;

    #ifdef STOP_n_COPY
    mlvalue* heap_a;
    mlvalue* heap_b;
    size_t heap_size;
    size_t alloc_ptr;
    #endif

    #ifdef MARK_n_SWEEP
    Bloc big_obj;
    Bloc freelist;
    Bloc page_list;
    size_t heap_size;
    size_t cur_size;
    #endif
} caml_domain_state;



/* The global state */
extern caml_domain_state* Caml_state;

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
