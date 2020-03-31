#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domain_state.h"
#include "mlvalues.h"
#include "parser.h"
#include "interp.h"
#include "alloc.h"
#include "gc.h"
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



// test
// int main(){
//     caml_init_domain();
//     printf("alloc_size=%ld\n", Caml_state->cur_size);
//     printf("=========1st allocation=============\n");
//     mlvalue data;
//     for(int i=0; i<2; i++){
//         Make_block(data, 3, BLOCK_T);
//         for(size_t j=0; j<Size(data); j++) Field(data, j) = Val_long(j+3);
//         Make_block(data, 5, BLOCK_T);
//         for(size_t j=0; j<Size(data); j++) Field(data, j) = Val_long(j+3);
//     }
//     printf("bigobj\n");
//     print_heap(Caml_state->big_obj);
//     printf("page_list\n");
//     print_heap(Caml_state->page_list);
//     printf("freelist\n");
//     print_heap(Caml_state->freelist);
//     printf("alloc_size=%ld\n", Caml_state->cur_size);   
//     printf("=========1er sweep=============\n");
//     sweep();
//     printf("bigobj\n");
//     print_heap(Caml_state->big_obj);
//     printf("page_list\n");
//     print_heap(Caml_state->page_list);
//     printf("freelist\n");
//     print_heap(Caml_state->freelist);
//     printf("alloc_size=%ld\n", Caml_state->cur_size);
//     printf("=========2nd allocation =============\n");
//     Make_block(data, 3, BLOCK_T);
//     for(size_t j=0; j<Size(data); j++) Field(data, j) = Val_long(j+10);
//     for(int i=0; i<3; i++){
//         Make_block(data, 1, BLOCK_T);
//         for(size_t j=0; j<Size(data); j++) Field(data, j) = Val_long(j+10);
//     }
//     printf("bigobj\n");
//     print_heap(Caml_state->big_obj);
//     printf("page_list\n");
//     print_heap(Caml_state->page_list);
//     printf("freelist\n");
//     print_heap(Caml_state->freelist);
//     printf("alloc_size=%ld\n", Caml_state->cur_size);
//     return 0;
// }



