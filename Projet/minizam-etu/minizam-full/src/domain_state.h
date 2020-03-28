#ifndef _DOMAIN_STATE_H
#define _DOMAIN_STATE_H

#include "mlvalues.h"
#include "gc.h"
#include "alloc.h"


#ifdef MARK_n_SWEEP
typedef struct _block_list* Block_list;
struct _block_list{
  mlvalue * page;
  Block_list next;
};
#endif


typedef struct _caml_domain_state {
  /* Stack */
  mlvalue* stack;
  unsigned int sp;
  mlvalue env;
  uint64_t alloc_ptr;

  #ifdef STOP_n_COPY
  mlvalue* heap_a;
  mlvalue* heap_b;
  size_t heap_size;
  #endif

  #ifdef MARK_n_SWEEP
  Block_list big_obj;
  Block_list freelist;
  mlvalue * page_actuel;
  #endif
} caml_domain_state;



/* The global state */
extern caml_domain_state* Caml_state;

/* Initialisation function for |Caml_state| */
void caml_init_domain();

#endif
