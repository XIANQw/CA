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
    Caml_state->alloc_ptr = 0;

    #ifdef STOP_n_COPY
    Caml_state->heap_size = (size_t)(Heap_size/sizeof(mlvalue));
    Caml_state->heap_a = malloc(Heap_size);
    Caml_state->heap_b = malloc(Heap_size);
    #endif

    #ifdef MARK_n_SWEEP
    Caml_state->big_obj = NULL;
    Caml_state->page_actuel = (mlvalue *)malloc(Page_size);
    Append_block_list(Caml_state->page_actuel, Caml_state->freelist);
    #endif

} 
