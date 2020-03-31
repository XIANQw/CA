#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domain_state.h"
#include "mlvalues.h"
#include "parser.h"
#include "interp.h"
#include "alloc.h"
#if defined STOP_n_COPY || defined MARK_n_SWEEP
#include "gc.h"
#endif

#ifdef GENERAL
#include "gc_general.h"
#endif
#include "bloc.h"

/* Note that Caml_state is allocated by caml_init_domain but never
   freed. You might therefore not want to repeatedly call
   eval_file... */
mlvalue eval_file(char* filename) {
  code_t* code = parse(filename);
  caml_init_domain();
  mlvalue ret = caml_interprete(code);
  free(code);
  return ret;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  mlvalue res = eval_file(argv[1]);
  // TODO: use getopt rather than this not-so-elegant strcmp.
  if (argc >= 3 && strcmp(argv[2], "-res") == 0) {
    char* res_str = val_to_str(res);
    printf("%s\n", res_str);
    free(res_str);
  }
}


/*
// test
int main(){
    caml_init_domain();
    printf("alloc_size=%ld\n", Caml_state->cur_size);
    printf("heap=%ld, alloc_ptr=%ld\n", Caml_state->heap_a, Caml_state->alloc_ptr);
    
    printf("=========1st allocation=============\n");
    mlvalue data0, data1;
    Make_empty_env(Caml_state->env);
    PUSH_STACK(Caml_state->env);
    for(int i=0; i<2; i++){
        Make_block(data0, 3, BLOCK_T);
        for(size_t j=0; j<Size(data0); j++) Field(data0, j) = Val_long(j+3);
        New_ptr(data0) = Val_long(0);
    }
    PUSH_STACK(data0);
    printf("heap=%ld, alloc_ptr=%ld\n", Caml_state->heap_a, Caml_state->alloc_ptr);
    for(unsigned i=0; i<Sp; i++){
        printf("Stack[%d]=%ld, %s, color=%ld, gene=%ld, tag=%ld\n", i, Stack[i], val_to_str(Stack[i]), Color(Stack[i]), Gene(Stack[i]), Tag(Stack[i]));
    }
    printf("1st gc\n");
    gc_general();
    printf("heap=%ld, alloc_ptr=%ld\n", Caml_state->heap_a, Caml_state->alloc_ptr);
    print_heap(Caml_state->page_list);
    print_heap(Caml_state->big_obj);
    for(unsigned i=0; i<Sp; i++){
        printf("Stack[%d]=%ld, %s, color=%ld, gene=%ld, tag=%ld\n", i, Stack[i], val_to_str(Stack[i]), Color(Stack[i]), Gene(Stack[i]), Tag(Stack[i]));
    }

    printf("=========2nd allocation=============\n");
    data1 = make_list(3);
    PUSH_STACK(data1);
    printf("heap=%ld, alloc_ptr=%ld\n", Caml_state->heap_a, Caml_state->alloc_ptr);
    for(unsigned i=0; i<Sp; i++){
        printf("Stack[%d]=%ld, %s, color=%ld, gene=%ld, tag=%ld\n", i, Stack[i], val_to_str(Stack[i]), Color(Stack[i]), Gene(Stack[i]), Tag(Stack[i]));
    }
    print_heap(Caml_state->page_list);
    print_heap(Caml_state->big_obj);
    gc_general();
    
    printf("2nd gc\n");
    for(unsigned i=0; i<Sp; i++){
        printf("Stack[%d]=%ld, %s, color=%ld, gene=%ld, tag=%ld\n", i, Stack[i], val_to_str(Stack[i]), Color(Stack[i]), Gene(Stack[i]), Tag(Stack[i]));
    }
    print_heap(Caml_state->page_list);
    print_heap(Caml_state->big_obj);
    
    printf("3rd gc\n");
    gc_general();
    for(unsigned i=0; i<Sp; i++){
        printf("Stack[%d]=%ld, %s, color=%ld, gene=%ld, tag=%ld\n", i, Stack[i], val_to_str(Stack[i]), Color(Stack[i]), Gene(Stack[i]), Tag(Stack[i]));
    }
    print_heap(Caml_state->page_list);
    print_heap(Caml_state->big_obj);
    
    return 0;
}
*/


