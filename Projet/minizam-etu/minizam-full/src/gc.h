#ifndef _GC_H
#define _GC_H

#include "mlvalues.h"

int gc(size_t new_heap_size);
mlvalue copy_obj(mlvalue obj);
#endif