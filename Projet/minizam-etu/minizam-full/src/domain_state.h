#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "gc.h"
#include "alloc.h"

#if (defined MARK_n_SWEEP) || (defined GENERAL) 
#include "bloc.h"
#endif


typedef struct _caml_domain_state {
  /* Stack */
    mlvalue* stack;
    unsigned int sp;
    mlvalue env;
    size_t heap_size;

    #if (defined STOP_n_COPY) || (defined GENERAL) 
    mlvalue* heap_a;
    mlvalue* heap_b;
    size_t alloc_ptr;
    #endif

    #if (defined MARK_n_SWEEP) || (defined GENERAL) 
    Bloc big_obj;
    Bloc freelist;
    Bloc page_list;
    size_t cur_size;
    #endif
} caml_domain_state;



/* The global state */
extern caml_domain_state* Caml_state;

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
