#ifndef _ALLOC_H
#define _ALLOC_H

#include "mlvalues.h"

mlvalue* caml_alloc(size_t size);
#define SIZE Caml_state->heap_size

#endif
