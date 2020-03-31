#include <stdlib.h>

#include "domain_state.h"
#include "config.h"
#include "mlvalues.h"
#include "gc.h"

caml_domain_state* Caml_state;

void caml_init_domain() {

    Caml_state = (caml_domain_state *)malloc(sizeof(caml_domain_state));
    Caml_state->stack = malloc(Stack_size);
    Caml_state->sp = 0;

    #if (defined STOP_n_COPY) || (defined GENERAL) 
    Caml_state->heap_size = (size_t)(Heap_size/sizeof(mlvalue));
    Caml_state->heap_a = malloc(Heap_size);
    Caml_state->heap_b = NULL;
    Caml_state->alloc_ptr = 0;
    #endif

    #if (defined MARK_n_SWEEP) || (defined GENERAL) 
    Caml_state->big_obj = NULL;
    Caml_state->freelist = NULL;
    Caml_state->page_list = NULL;
    Caml_state->cur_size = 0;
    #endif
    
    #ifdef MARK_n_SWEEP
    Caml_state->heap_size =  128 * KB;
    #endif

} 
