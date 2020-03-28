#include "mlvalues.h"
#include "gc.h"
#include "domain_state.h"
#include <stdlib.h>
#include "alloc.h"
#include <stdio.h>

#ifdef MARK_n_SWEEP
/*
    MARK & SWEEP: allouer objet的时候就在最后一个case上存放他们所在的bloc的地址.
    1. definit Bloc (ok)
    2. delete_bloc (ok)
    3. mark
    扫描stack, 把objet vivant所在的bloc的tag都从 N_MARK_T 变成 MARK_T. 
    然后扫描freelist, 如果 bloc 不是 MARK_T, 就free掉 bloc->page, 然后删除掉bloc.
*/

void gc_mark_sweep(){
    mark(); sweep();
}


void sweep(){
    Bloc tmp = Caml_state->freelist;
    while(tmp){
        if(tmp->tag == N_MARK_T){
            tmp = delete_bloc(tmp, 1);
            Caml_state->cur_size -= Page_size;
        }
        else tmp = tmp->next;
    }
    tmp = Caml_state->big_obj;
    while(tmp){
        if(tmp->tag == N_MARK_T){
            size_t s = Size(Val_ptr(tmp->page+1));
            Caml_state->cur_size -= (s+2)*sizeof(mlvalue);
            tmp = delete_bloc(tmp, 0);
        }
        else tmp = tmp->next;
    }
}

void mark(){
    unsigned int sp = Caml_state->sp;
    mlvalue * stack = Caml_state->stack;
    for(unsigned int i=0; i<sp; i++){
        mark_bloc(stack[i]);
    }
}


void mark_bloc(mlvalue obj){
    if(Is_long(obj)) return;
    Bloc tmp = BLOC(obj);
    tmp->tag = MARK_T;
    for(unsigned int i=0; i<Size(obj); i++){
        mark_bloc(Field(obj, i));
    }
}


Bloc delete_bloc(Bloc bloc, int flag){
    Bloc prev = bloc->prev, next = bloc->next;
    if(next) next->prev = prev;
    if(prev){
        prev->next = next;
    }else{
        if(flag) Caml_state->freelist = next;
        else Caml_state->big_obj = next;
    } 
    free(bloc->page);
    bloc->page = NULL;
    free(bloc);
    return next;
}

#endif



/*
    STOP & COPY: 扫描stack, 对每个值判断是否是ptr, 如果是就判断ptr的tag是否 FWD_PTR_T. 
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




