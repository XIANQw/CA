#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domain_state.h"
#include "mlvalues.h"
#include "parser.h"
#include "interp.h"
#include "alloc.h"
#include "gc.h"

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

// int main(int argc, char** argv) {
//   if (argc < 2) {
//     fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
//     exit(EXIT_FAILURE);
//   }
//   mlvalue res = eval_file(argv[1]);
//   // TODO: use getopt rather than this not-so-elegant strcmp.
//   if (argc >= 3 && strcmp(argv[2], "-res") == 0) {
//     char* res_str = val_to_str(res);
//     printf("%s\n", res_str);
//     free(res_str);
//   }
// }



// test
int main(){
  caml_init_domain();
  printf("page_acutel=%ld\n", Caml_state->page_actuel);
  mlvalue data;
  Make_block(data, 1, BLOCK_T);
  Field0(data) = Val_long(1);
  printf("page_acutel=%ld\n", Caml_state->page_actuel);
  printf("alloc_ptr=%ld\n", Caml_state->alloc_ptr);
  Make_block(data, 1, BLOCK_T);
  Field0(data) = Val_long(2);
  printf("page_acutel=%ld\n", Caml_state->page_actuel);
  printf("alloc_ptr=%ld\n", Caml_state->alloc_ptr);
  Make_block(data, 1, BLOCK_T);
  Field0(data) = Val_long(3);
  printf("page_acutel=%ld\n", Caml_state->page_actuel);
  printf("alloc_ptr=%ld\n", Caml_state->alloc_ptr);
  Make_block(data, 3, BLOCK_T);
  printf("page_acutel=%ld\n", Caml_state->page_actuel);
  printf("alloc_ptr=%ld\n", Caml_state->alloc_ptr);
  for(int i=0; i<3; i++) Field(data, i) = Val_long(10+i);
  printf("data1=%s\n", val_to_str(Val_ptr(Caml_state->freelist->next->page + 1)));
  printf("data2=%s\n", val_to_str(Val_ptr(Caml_state->freelist->next->page + 4)));
  printf("data3=%s\n", val_to_str(Val_ptr(Caml_state->freelist->page + 1)));
  printf("data4=%s\n", val_to_str(Val_ptr(Caml_state->big_obj->page + 1)));
}




