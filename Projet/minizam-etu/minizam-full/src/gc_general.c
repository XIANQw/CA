#include "gc_general.h"

#ifdef GENERAL


void gc_general(){
    gc_stop_copy();
    gc_mark_sweep();
}

/*
    GC general:
    1. parcourir stack, stack[i]:
    young, white, 那么copy到新heap上然后Color变成white->gray.
    young, gray, 那么copy到新heap上然后Color变成Black.
    young, black, 那么copy到page_list或big_obj上, Color变成white, generation变成old.
    old, white -> old, black
*/

void gc_stop_copy(){
    unsigned int sp = Caml_state->sp;
    mlvalue * stack = Caml_state->stack;
    mlvalue * tmp = Caml_state->heap_b;
    if(tmp) free(tmp);
    Caml_state->heap_b = Caml_state->heap_a;
    Caml_state->heap_a = malloc(SIZE * sizeof(mlvalue));
    Caml_state->alloc_ptr = 0;
    Caml_state->env = copy_obj_young(Caml_state->env);
    for(unsigned int i=0; i < sp; i++){
        stack[i] = copy_obj_young(stack[i]);
    }
}

mlvalue copy_obj_young(mlvalue obj){
    mlvalue new;
    if(Is_long(obj)) return obj;

    if(Gene(obj) == OLD) return obj;
    if(Tag(obj) == FWD_PTR_T) return New_ptr(obj);
    if(Gene(obj) == YOUNG && Color(obj) == BLACK){
        new = Val_ptr(caml_alloc_old(Size(obj) + ADD_SIZE));
        Field0(new) = Make_header(Size(obj), WHITE, OLD, Tag(obj));
    }else{
        new = Val_ptr(caml_alloc(Size(obj) + ADD_SIZE)); 
        Field0(new) = Make_header(Size(obj), (Color(obj) + 1), YOUNG, Tag(obj));
        
    }
    new = Val_ptr(Ptr_val(new)+1);
    for(unsigned int i=0; i < Size(obj); i++){
        Field(new, i) = copy_obj_young(Field(obj, i));
    }
    Hd_val(obj) = Make_header(Size(obj), Color(obj), Gene(obj), FWD_PTR_T);
    New_ptr(obj) = new;
    return new;
}

mlvalue copy_obj(mlvalue obj){
    mlvalue new;
    if(Is_long(obj)) return obj;
    if(Gene(obj) == OLD) return obj;
    if(Tag(obj) == FWD_PTR_T) return New_ptr(obj);
    new = Val_ptr(caml_alloc(Size(obj) + ADD_SIZE));
    Field0(new) = Make_header(Size(obj), Color(obj), Gene(obj), Tag(obj));
    new = Val_ptr(Ptr_val(new)+1);
    for(unsigned int i=0; i < Size(obj); i++){
        Field(new, i) = copy_obj(Field(obj, i));
    }
    Hd_val(obj) = Make_header(Size(obj), Color(obj), Gene(obj), FWD_PTR_T);
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
                    // Caml_state->cur_size -= (Size(obj)+1)*sizeof(mlvalue);
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
            // Caml_state->cur_size -= (Size(obj)+1)*sizeof(mlvalue);
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
                Hd_val(obj) = Make_header(Size(obj), WHITE, Gene(obj), Tag(obj));
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
        Hd_val(obj) = Make_header(Size(obj), WHITE, Gene(obj), Tag(obj));
        pt = pt->next;
    }
}

void mark_obj(mlvalue obj){
    if(Is_long(obj)) return;
    if(Gene(obj) == YOUNG) return ;
    Hd_val(obj) = Make_header(Size(obj), BLACK, Gene(obj), Tag(obj));
    for(unsigned int i=0; i<Size(obj); i++){
        mark_obj(Field(obj, i));
    }
}

void gc_mark_sweep(){
    init_big_obj();
    init_heap();
    mark(); 
    sweep();
}


#endif