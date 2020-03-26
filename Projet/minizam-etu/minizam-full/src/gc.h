#ifndef _GC_H
#define _GC_H

#include "mlvalues.h"

int gc(unsigned int sp, mlvalue * stack);
mlvalue copy_obj(mlvalue obj);
#endif