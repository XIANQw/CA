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
    3. init
    扫描espace-pagine, 把所有objet变成WHITE.
    4. mark
    扫描stack, 把objet vivant的objet 从 WHITE 变成 BLACK.
    5. sweep
    扫描espace-pagine, 把 WHITE 的objet的 adress和size 组成Bloc加入freelist中
    6. alloc
    优先从freelist中找, 如果有能用的就返回bloc->page用. bloc要是满了就从freelist中删掉, 不然freelist会越来越长
*/

void sweep_page(){
    Bloc pt = Caml_state->page_list;
    while(pt){
        mlvalue * tmp = pt->page+1;
        size_t cpt = 0;
        while(cpt < pt->size){
            if(*(tmp+cpt) == 0) cpt++;
            else{
                mlvalue obj = Val_ptr(tmp+cpt);
                if(Color(obj) == WHITE){
                    Bloc bloc;
                    Append_bloc_list(Ptr_val(obj)-1, bloc, Size(obj)+1, Caml_state->freelist);
                    Caml_state->cur_size -= (Size(obj)+1)*sizeof(mlvalue);
                }
                cpt += Size(obj) + 1;
            }
        }
        pt = pt->next;
    }
}

void sweep_bigobj(){
    Bloc pt = Caml_state->big_obj;
    while(pt){
        mlvalue obj = Val_ptr(pt->page+1);
        if(Color(obj) == WHITE){
            Bloc bloc;
            Append_bloc_list(Ptr_val(obj) - 1, bloc, Size(obj)+1, Caml_state->freelist);
            pt = delete_bloc(pt, 0);
            Caml_state->cur_size -= (Size(obj)+1)*sizeof(mlvalue);
        }else{
            pt = pt->next;
        }
    }
}

void sweep(){
    sweep_bigobj();
    sweep_page();
}

void mark(){
    unsigned int sp = Caml_state->sp;
    mlvalue * stack = Caml_state->stack;
    for(unsigned int i=0; i<sp; i++){
        mark_obj(stack[i]);
    }
}

void init_heap(){
    Bloc pt = Caml_state->page_list;
    while(pt){
        mlvalue * tmp = pt->page+1;
        size_t cpt = 0;
        while(cpt < pt->size){
            if(*(tmp+cpt) == 0){
                cpt += 1;
            }else{
                mlvalue obj = Val_ptr(tmp+cpt);
                Hd_val(obj) = Make_header(Size(obj), WHITE, Tag(obj));
                cpt += Size(tmp+cpt) + 1;
            }
        }
        pt = pt->next;
    }
}

void init_big_obj(){
    Bloc pt= Caml_state->big_obj;
    while(pt){
        mlvalue obj = Val_ptr(pt->page+1);
        Hd_val(obj) = Make_header(Size(obj), WHITE, Tag(obj));
        pt = pt->next;
    }
}

void mark_obj(mlvalue obj){
    if(Is_long(obj)) return;
    Hd_val(obj) = Make_header(Size(obj), BLACK, Tag(obj));
    for(unsigned int i=0; i<Size(obj); i++){
        mark_obj(Field(obj, i));
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
    bloc->page = NULL;
    free(bloc);
    return next;
}

void gc_mark_sweep(){
    init_big_obj();
    init_heap();
    mark(); 
    sweep();
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




