#ifndef _BLOC_H
#define _BLOC_H

#include "config.h"

#if (defined MARK_n_SWEEP) || (defined GENERAL) 

#include <stdio.h>
#include <stdlib.h>
#include "mlvalues.h"


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

Bloc delete_bloc(Bloc bloc, int flag);
void print_bloc(Bloc tmp);
void print_bloc_list(Bloc bloclist);
void print_page(Bloc bloc);
void print_heap(Bloc heap);
#endif

#endif