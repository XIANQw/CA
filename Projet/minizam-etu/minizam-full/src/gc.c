#include "mlvalues.h"
#include "gc.h"
#include "domain_state.h"
#include <stdlib.h>
#include "alloc.h"
#include <stdio.h>

#ifdef MARK_n_SWEEP
/*
    MARK & SWEEP:
    1. definit Bloc (ok)
    2. delete_bloc (ok)
    Si un bloc de freelist n'a plus d'espace, supprimer ce bloc depuis freelist et le libérer.
    3. init (ok)
    Parcourir espace-pagine, changer tous les colors d'objet à WHITE.
    4. mark (ok)
    Parcourir stack, changer les tags des objets vivants BLACK depuis WHITE.
    5. sweep (ok)
    Parcourir espace-pagine, ajouter les adresses des objets dont le color est WHITE à freelist.
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

void gc_mark_sweep(){
    init_big_obj();
    init_heap();
    mark(); 
    sweep();
}

#endif



/*
    STOP & COPY: 
    1. Parcourir stack, vérifier si stack[i] est pointeur. Si stack[i] est pointeur, alors vérifier si son tag est FWD_PTR_T. 
    2. Sinon créer sa copie sur heap_b, changer son tag à FWD_PTR_T, faire stack[i] = copie.
    Ex: block est [header|0, 1, 2, 3, NULL], s'occupe 6 cases de mlvalue. header: |size(4)+color(white)+tag(BLOCK_t)|. 
    On créer un copie de block sur heap_b, et modifie le tag du block qui se trouve sur heap_a, et stocker l'adresse de nouvelle version
    à le dernier case de block de vieux version. [size+color+FWD_PTR_T|0, 1, 2, 3, newptr].
*/
#ifdef STOP_n_COPY
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

#endif




