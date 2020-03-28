#include <stdlib.h>

#include "alloc.h"
#include "config.h"
#include "mlvalues.h"
#include "domain_state.h"
#include <stdio.h>

#ifdef STOP_n_COPY

/*
  Allouer les espaces mémoires directement sur notre tas qu'on a malloc au moment initial.
  C'est juste rendre l'adresse de case du tas et incrémenter alloc_ptr.
*/


mlvalue* caml_alloc(size_t size) {
    int had_gc = 0;
    if (Caml_state->alloc_ptr + size > SIZE){
        gc();
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
  Paginer le mémoire et allouer les espaces mémoire sur le page actuel. S'il y a assez de espace.
*/
mlvalue * caml_alloc(size_t size){
    mlvalue * res;
    // Si allouer un gros objet, faire malloc individuel
    if(size * sizeof(mlvalue) > (size_t)BIG_OBJ){
        res = (mlvalue *)malloc(size * sizeof(mlvalue));
        Append_bloc_list(res, Caml_state->big_obj);
        Caml_state->big_obj->alloc_ptr += size;
        *(res+size-1) = (mlvalue)bloc;
        return res;
    }
    // Exception 1: Page_actuel n'a pas assez d'espace, cherche un bloc util dans freelist
    Bloc cur_bloc = Caml_state->freelist;
    while(cur_bloc && cur_bloc->alloc_ptr + size > Page_size/sizeof(mlvalue)){
        cur_bloc = cur_bloc->next;
    }
    if(cur_bloc == NULL){
        mlvalue * newpage = (mlvalue *)malloc(Page_size);
        Append_bloc_list(newpage, Caml_state->freelist);
        cur_bloc = Caml_state->freelist;
    }
    res = cur_bloc->page + cur_bloc->alloc_ptr;
    cur_bloc->alloc_ptr += size;
    *(res+size-1) = (mlvalue)cur_bloc;
    return res;
}
#endif


