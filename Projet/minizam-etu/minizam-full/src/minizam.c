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


mlvalue make_list(int n){
    mlvalue head;
    if(!n){
        Make_empty_block(head, BLOCK_T);
        return head;
    }
    Make_block(head, 2, BLOCK_T);
    Field0(head) = Val_long(3);
    Field1(head) = make_list(n - 1);
    return head;
}

/*
// test
int main(){
    caml_init_domain();
    mlvalue data = make_list(5);
    mlvalue data2, data3;
    for(int i=0; i < 4; i++){
        Make_block(data2, 2, BLOCK_T);
        Field0(data2) = Val_long(1); Field1(data2) = Val_long(2);
    }
    for(int i=0; i < 4; i++){
        Make_block(data3, 5, BLOCK_T);
        for(int i=0; i<5; i++) Field(data3, i) = Val_long(i+10);
    }
    printf("data=%ld\n", data);
    printf("data2=%ld\n", data2);
    mark_bloc(data); 
    mark_bloc(data2);
    Bloc tmp = Caml_state->freelist;
    printf("freelist=%ld\n", tmp);
    while(tmp){
        printf("tmp=%ld, tmp->page=%ld, ptr=%ld, tag=%ld, prev=%ld, next=%ld\n", tmp, tmp->page, tmp->alloc_ptr, tmp->tag, tmp->prev, tmp->next);
        tmp = tmp->next;
    }
    tmp = Caml_state->big_obj;
    printf("bigobj=%ld\n", tmp);
    while(tmp){
        printf("tmp=%ld, tmp->page=%ld, ptr=%ld, tag=%ld, prev=%ld, next=%ld\n", tmp, tmp->page, tmp->alloc_ptr, tmp->tag, tmp->prev, tmp->next);
        tmp = tmp->next;
    }    
    sweep();
    tmp = Caml_state->freelist;
    printf("freelist=%ld\n", tmp);
    while(tmp){
        printf("tmp=%ld, tmp->page=%ld, ptr=%ld, tag=%ld, prev=%ld, next=%ld\n", tmp, tmp->page, tmp->alloc_ptr, tmp->tag, tmp->prev, tmp->next);
        tmp = tmp->next;
    }
    tmp = Caml_state->big_obj;
    printf("bigobj=%ld\n", tmp);
    while(tmp){
        printf("tmp=%ld, tmp->page=%ld, ptr=%ld, tag=%ld, prev=%ld, next=%ld\n", tmp, tmp->page, tmp->alloc_ptr, tmp->tag, tmp->prev, tmp->next);
        tmp = tmp->next;
    } 
    return 0;
}
*/



