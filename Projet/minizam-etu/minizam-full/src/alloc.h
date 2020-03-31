#ifndef _ALLOC_H
#define _ALLOC_H

#include "mlvalues.h"
#include "gc.h"
#include "config.h"

mlvalue* caml_alloc(size_t size);

#define SIZE Caml_state->heap_size

#if (defined MARK_n_SWEEP) || (defined GENERAL) 
#include "bloc.h"
#define BIG_OBJ (Page_size/2)
#endif


#endif
