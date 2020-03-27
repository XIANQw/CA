#ifndef _GC_H
#define _GC_H

#include "mlvalues.h"
#define STOP_n_COPY

#ifdef STOP_n_COPY
    void gc();
    mlvalue copy_obj(mlvalue obj);
    void resize();
#endif

#endif