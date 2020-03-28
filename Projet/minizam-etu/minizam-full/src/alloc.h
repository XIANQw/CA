#ifndef _ALLOC_H
#define _ALLOC_H

#include "mlvalues.h"
#include "gc.h"
#include "config.h"

mlvalue* caml_alloc(size_t size);
#ifdef STOP_n_COPY
#define SIZE Caml_state->heap_size
#endif

#ifdef MARK_n_SWEEP
#define BIG_OBJ (Page_size/2)
#define Malloc_block_list (Block_list)malloc(sizeof(struct _block_list))
#define Append_block_list(newpage, list) \
    Block_list bloc = Malloc_block_list; \
    bloc->page = (mlvalue *)newpage; \
    bloc->next = list; \
    list = bloc
#endif


#endif
