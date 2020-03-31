#include "gc_general.h"


void gc_stop_copy(){
    unsigned int sp = Caml_state->sp;
    mlvalue * stack = Caml_state->stack;
    mlvalue * tmp = Caml_state->heap_b;
    free(tmp);
    Caml_state->heap_b = Caml_state->heap_a;
    Caml_state->heap_a = malloc(SIZE * sizeof(mlvalue));
    Caml_state->alloc_ptr = 0;
    Caml_state->env = copy_obj(Caml_state->env);
    for(unsigned int i=0; i < sp; i++){
        stack[i] = copy_obj(stack[i]);
    }
}

mlvalue copy_obj(mlvalue obj){
    mlvalue new;
    if(Is_long(obj)) return obj;
    if(Tag(obj) == FWD_PTR_T) return New_ptr(obj);
    Make_block(new, Size(obj), Tag(obj));
    for(unsigned int i=0; i < Size(obj); i++){
        Field(new, i) = copy_obj(Field(obj, i));
    }
    Hd_val(obj) = Make_header(Size(new), WHITE, FWD_PTR_T);
    New_ptr(obj) = new;
    return new;
}

void resize(){
    size_t new_size = Caml_state->heap_size;
    mlvalue * tmp = Caml_state->heap_a;
    Caml_state->heap_a = malloc(new_size * sizeof(mlvalue));
    Caml_state->alloc_ptr = 0;
    unsigned int sp = Caml_state->sp;
    mlvalue * stack = Caml_state->stack;
    Caml_state->env = copy_obj(Caml_state->env);
    for(unsigned int i=0; i < sp; i++){
        stack[i] = copy_obj(stack[i]);
    }
    free(tmp);
}