#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"

#include <stdio.h>

#ifdef GENERAL

#define Stack Caml_state->stack

mlvalue* caml_alloc(size_t size) {
    int had_gc = 0;
    if (Caml_state->alloc_ptr + size > SIZE){
        gc_general();
        had_gc = 1;
    }
    if(had_gc && Caml_state->alloc_ptr >= SIZE/2){
        SIZE = (size_t)(SIZE * 1.5);
        resize();
        // printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
    }
    // else if(had_gc && Caml_state->alloc_ptr < SIZE/2){
    //     SIZE = (size_t)(SIZE/1.5);
    //     resize();
    //     printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
    // }
    mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
    Caml_state->alloc_ptr += size;
    return res;
}

mlvalue* caml_alloc_old(size_t size) {
    mlvalue * res;
    Bloc bloc;
    int had_gc = 0;
    // Allouer un gros objet, faire malloc individuel
    if(size * sizeof(mlvalue) > (size_t)BIG_OBJ){
        res = (mlvalue *)malloc(size * sizeof(mlvalue));
        Append_bloc_list(res, bloc, size, Caml_state->big_obj);
        Caml_state->big_obj->ptr += size;
        Caml_state->cur_size += size * sizeof(mlvalue);
        return res;
    }
    // Cherche un bloc utile dans freelist
    Bloc cur_bloc = Caml_state->freelist;
    while(cur_bloc && cur_bloc->ptr + size > cur_bloc->size){
        cur_bloc = cur_bloc->next;
    }
    // Cas 1: Il n'y a pas de bloc disponible
    if(cur_bloc == NULL){
        mlvalue * newpage = (mlvalue *)malloc(Page_size);
        memset(newpage, 0, Page_size);
        size_t size_bloc = Page_size/sizeof(mlvalue);
        Append_bloc_list(newpage, bloc, size_bloc, Caml_state->freelist);
        Append_bloc_list(newpage, bloc, size_bloc, Caml_state->page_list);
        cur_bloc = Caml_state->freelist;
    }
    res = cur_bloc->page + cur_bloc->ptr;
    cur_bloc->ptr += size;
    Caml_state->cur_size += size*sizeof(mlvalue);
    if(cur_bloc->ptr == cur_bloc->size) delete_bloc(cur_bloc, 1);
    return res;
}

#endif


#ifdef STOP_n_COPY

/*
  Allouer les espaces mémoires directement sur notre tas qu'on a malloc au moment initial.
  C'est juste rendre l'adresse de case du tas et incrémenter alloc_ptr.
*/


mlvalue* caml_alloc(size_t size) {
    int had_gc = 0;
    if (Caml_state->alloc_ptr + size > SIZE){
        gc_stop_copy();
        had_gc = 1;
        // printf("Gc: alloc_ptr=%ld, alloc_size=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, size, SIZE);
    }
    if(had_gc && Caml_state->alloc_ptr >= SIZE/2){
        SIZE = (size_t)(SIZE * 1.5);
        resize();
        // printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
    }
    else if(had_gc && Caml_state->alloc_ptr < SIZE/2){
        SIZE = (size_t)(SIZE/1.5);
        resize();
        // printf("resize: alloc_ptr=%ld, SIZE=%ld\n", Caml_state->alloc_ptr, SIZE);
    }
    mlvalue * res = Caml_state->heap_a + Caml_state->alloc_ptr;
    Caml_state->alloc_ptr += size;
    return res;
}
#endif

#ifdef MARK_n_SWEEP
/*
  Chercher un bloc utile depuis freelist pour les objets normaux.
*/
mlvalue * caml_alloc(size_t size){
    mlvalue * res;
    Bloc bloc;
    int had_gc = 0;
    // Allouer un gros objet, faire malloc individuel
    if(size * sizeof(mlvalue) > (size_t)BIG_OBJ){
        res = (mlvalue *)malloc(size * sizeof(mlvalue));
        Append_bloc_list(res, bloc, size, Caml_state->big_obj);
        Caml_state->big_obj->ptr += size;
        Caml_state->cur_size += size * sizeof(mlvalue);
        return res;
    }
    // Cherche un bloc utile dans freelist
    Bloc cur_bloc = Caml_state->freelist;
    while(cur_bloc && cur_bloc->ptr + size > cur_bloc->size){
        cur_bloc = cur_bloc->next;
    }
    // Cas 1: Il n'y a pas de bloc disponible
    if(cur_bloc == NULL){
        if(Caml_state->cur_size > SIZE){
            // printf("cur_size=%ld KB\n", Caml_state->cur_size/KB);
            gc_mark_sweep();
            had_gc = 1;
            // printf("Gc: cur_size=%ld KB\n", Caml_state->cur_size/KB);
        }
        if(had_gc && Caml_state->cur_size > SIZE/2){
            SIZE *= 1.5;
        }
        else if(had_gc && Caml_state->cur_size < SIZE/2){
            SIZE /= 1.5;
        }
        mlvalue * newpage = (mlvalue *)malloc(Page_size);
        memset(newpage, 0, Page_size);
        size_t size_bloc = Page_size/sizeof(mlvalue);
        Append_bloc_list(newpage, bloc, size_bloc, Caml_state->freelist);
        Append_bloc_list(newpage, bloc, size_bloc, Caml_state->page_list);
        cur_bloc = Caml_state->freelist;
    }
    res = cur_bloc->page + cur_bloc->ptr;
    cur_bloc->ptr += size;
    Caml_state->cur_size += size*sizeof(mlvalue);
    if(cur_bloc->ptr == cur_bloc->size) delete_bloc(cur_bloc, 1);
    return res;
}
#endif


