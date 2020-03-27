#include "mlvalues.h"
#include "gc.h"
#include "domain_state.h"
#include <stdlib.h>
#include "alloc.h"
#include <stdio.h>

#ifdef MARK_n_SWEEP

#endif



/*
 GC: 扫描stack, 对每个值判断是否是ptr, 如果是就判断ptr的tag是否 FWD_PTR_T. 
    不是FWD_PTR_T: 在to-space上创建一个copy, 然后把from_space上的版本的tag改成 FWD_PTR_T, 然后把指针指到to-space的Objet上.
    例如: block是 [header|0, 1, 2, 3, NULL], 在堆上占用6个mlvalue的大小. header: |size(4)+color(white)+tag(BLOCK_t)|, tag= block_t, 我们可以在heap_b上创建这个block的copy,
    然后把老的block改成 [size+color+FWD_PTR_T|0, 1, 2, 3, newptr]. 这样对于之后stack上指向老objet的指针, 只需要让他的值变成老指针值向的值, 即new pointer.
*/
#ifdef STOP_n_COPY
void gc(){
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

#endif




