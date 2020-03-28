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
    printf("freelist=%ld\n", Caml_state->freelist);
    mlvalue data;
    Make_block(data, 10, BLOCK_T);
    for(int i=0; i<10; i++){
        Field(data, i) = Val_long(i+10);
    }
    printf("big_obj=%ld\n", Caml_state->big_obj);
    printf("data_bloc=%ld\n", BLOC(data));
    Make_block(data, 1, BLOCK_T);
    Field0(data) = Val_long(10);
    Make_block(data, 2, BLOCK_T);
    Field0(data) = Val_long(2); Field1(data)=Val_long(3);
    Bloc tmp = Caml_state->freelist;
    while(tmp){
        printf("tmp->page=%ld, ptr=%ld\n", tmp->page, tmp->alloc_ptr);
        tmp = tmp->next;
    }
    return 0;
}




