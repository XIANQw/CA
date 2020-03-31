#ifndef _ALLOC_H
#define _ALLOC_H

#include "mlvalues.h"

#include "config.h"

#if (defined MARK_n_SWEEP) || (defined STOP_n_COPY)
#include "gc.h"
#endif

#ifdef GENERAL
#include "gc_general.h"
mlvalue* caml_alloc(size_t size);
mlvalue* caml_alloc_old(size_t size);
#endif


#if (defined MARK_n_SWEEP) || (defined STOP_n_COPY)
mlvalue* caml_alloc(size_t size);
#endif

#define SIZE Caml_state->heap_size

#if (defined MARK_n_SWEEP) || (defined GENERAL) 
#include "bloc.h"
#define BIG_OBJ (Page_size/2)
#endif


#endif
