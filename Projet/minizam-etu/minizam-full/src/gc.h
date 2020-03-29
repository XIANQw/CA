#ifndef _GC_H
#define _GC_H
#include <string.h>
#include "mlvalues.h"
#include "config.h"

#ifdef MARK_n_SWEEP
typedef struct _bloc * Bloc;

struct _bloc{
    mlvalue * page;
    size_t size;
    size_t ptr;
    Bloc prev; 
    Bloc next;
};

#define Malloc_bloc (Bloc)malloc(sizeof(struct _bloc))


#define Append_bloc_list(newpage, bloc, size_bloc, list) \
    bloc = Malloc_bloc; \
    bloc->page = (mlvalue *)newpage; \
    bloc->ptr = 0; \
    bloc->size = size_bloc; \
    bloc->next = list; \
    bloc->prev = NULL; \
    if(list) list->prev = bloc;\
    list = bloc

void sweep();
Bloc delete_bloc(Bloc bloc, int flag);
void mark_obj(mlvalue m);
void init_heap();
void gc_mark_sweep();

#endif

#ifdef STOP_n_COPY
    void gc();
    mlvalue copy_obj(mlvalue obj);
    void resize();
#endif




#endif

