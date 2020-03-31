#ifndef _GC_H
#define _GC_H
#include <string.h>
#include "mlvalues.h"
#include "config.h"

#ifdef MARK_n_SWEEP
#include "bloc.h"

void sweep();
void mark_obj(mlvalue m);
void init_heap();
void gc_mark_sweep();

#endif

#ifdef STOP_n_COPY
    void gc_stop_copy();
    mlvalue copy_obj(mlvalue obj);
    void resize();
#endif




#endif

