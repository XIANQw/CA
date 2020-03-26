#ifndef _INTERP_H
#define _INTERP_H

/* Helpers to manipulate the stack. Note that |sp| always point to the
   first empty element in the stack; hence the prefix -- in POP, but
   postfix ++ in PUSH. */
#define POP_STACK() stack[--sp]
#define PUSH_STACK(x) stack[sp++] = x
#include "mlvalues.h"

mlvalue caml_interprete(code_t* prog);

#endif /* _INTERP_H */
