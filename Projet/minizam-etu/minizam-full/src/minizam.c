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

mlvalue make_list(int size){
  mlvalue res;
  if(size == 0){
    Make_empty_block(res, BLOCK_T);
    return res;
  }
  Make_block(res, 2, BLOCK_T);
  Field0(res) = Val_long(1);
  Field1(res) = make_list(size - 1);
  return res;
}



// test
// int main(){
//   caml_init_domain();
//   printf("=====test copy linked====\n");
//   printf("heap_a=%ld\n", Caml_state->heap_a);
//   printf("heap_b=%ld\n", Caml_state->heap_b);
//   mlvalue old0 = make_list(3);
//   printf("old0=[%ld, %ld]\n", Long_val(Field0(old0)), Field1(old0));
//   mlvalue old1 = Field1(old0);
//   printf("old1=[%ld, %ld]\n", Long_val(Field0(old1)), Field1(old1));
//   mlvalue old2 = Field1(old1);
//   printf("old2=[%ld, %ld]\n", Long_val(Field0(old2)), Field1(old2));
//   mlvalue env; Make_env(env, 1);
//   Field0(env) = old0;
//   printf("env=%s\n", val_to_str(env));  
//   mlvalue * tmp = Caml_state->heap_a;
//   Caml_state->heap_a = Caml_state->heap_b;
//   Caml_state->heap_b = tmp;
//   Caml_state->alloc_ptr = 0;
//   mlvalue new0 = copy_obj(old0);
//   printf("New_ptr(old0)=%ld\n", New_ptr(old0));
//   printf("new0=%ld\n", new0);
//   printf("new0=[%ld, %ld]\n", Long_val(Field0(new0)), Field1(new0));
//   mlvalue new1= Field1(new0);
//   printf("new1=[%ld, %ld]\n", Long_val(Field0(new1)), Field1(new1));
//   mlvalue new2= Field1(new1);
//   printf("new2=[%ld, %ld]\n", Long_val(Field0(new2)), Field1(new2));
//   printf("new_=%s\n", val_to_str(new0));
//   mlvalue nenv = copy_obj(env);
//   printf("nenv[0]=%ld\n", Field0(nenv));
//   printf("nenv=%s\n", val_to_str(nenv));
// }




