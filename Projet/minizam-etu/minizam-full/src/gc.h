#ifndef _GC_H
#define _GC_H

#include "mlvalues.h"

// #define STOP_n_COPY

#define MARK_n_SWEEP

#ifdef MARK_n_SWEEP
typedef struct _bloc * Bloc;
typedef enum {MARK_T, N_MARK_T} bloc_tag;

struct _bloc{
    mlvalue * page;
    bloc_tag tag;
    size_t alloc_ptr;
    Bloc prev; 
    Bloc next;
};

#define Malloc_bloc (Bloc)malloc(sizeof(struct _bloc))

// Quel bloc cet objet se trouve
#define BLOC(obj) (Bloc)(New_ptr(obj))

#define Append_bloc_list(newpage, list) \
    Bloc bloc = Malloc_bloc; \
    bloc->page = (mlvalue *)newpage; \
    bloc->tag = N_MARK_T; \
    bloc->alloc_ptr = 0; \
    bloc->next = list; \
    bloc->prev = NULL; \
    if(list) list->prev = bloc;\
    list = bloc

void sweep();
Bloc delete_bloc(Bloc bloc, int flag);
void mark_bloc(mlvalue m);
void gc_mark_sweep();

#endif

#ifdef STOP_n_COPY
    void gc();
    mlvalue copy_obj(mlvalue obj);
    void resize();
#endif




#endif

