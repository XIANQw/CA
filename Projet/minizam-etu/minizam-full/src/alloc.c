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
  if(size * sizeof(mlvalue) > BIG_OBJ){
    res = (mlvalue *)malloc(size * sizeof(mlvalue));
    printf("res=%ld\n", res);
    Append_block_list(res, Caml_state->big_obj);
    return res;
  }
  // Exception 1: Page_actuel n'a pas assez d'espace
  if(Caml_state->alloc_ptr + size > Page_size/sizeof(mlvalue)){
    Caml_state->alloc_ptr = 0; // init alloc_ptr
    Caml_state->page_actuel = (mlvalue *)malloc(Page_size); // Creer un bloc et ajoute dans freelist
    Append_block_list(Caml_state->page_actuel, Caml_state->freelist);
  }
  res = Caml_state->page_actuel + Caml_state->alloc_ptr;
  Caml_state->alloc_ptr += size;
  return res;
}
#endif


