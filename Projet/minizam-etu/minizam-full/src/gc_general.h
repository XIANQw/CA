#ifndef _GC_GENERAL_H
#define _GC_GENERAL_H

#include "config.h"
#ifdef GENERAL

#include "mlvalues.h"
#include "domain_state.h"

void gc_general();
void gc_mark_sweep();
void mark_obj(mlvalue obj);


void gc_stop_copy();
void resize();
mlvalue copy_obj_young(mlvalue obj);
mlvalue copy_obj(mlvalue obj);

#endif
#endif