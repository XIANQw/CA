#include "bloc.h"
#include "domain_state.h"

#ifdef MARK_n_SWEEP

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

void print_bloc(Bloc tmp){
    printf("tmp=%ld, tmp->page=%ld, ptr=%ld, size=%ld, prev=%ld, next=%ld\n", tmp, tmp->page, tmp->ptr, tmp->size, tmp->prev, tmp->next);
}
void print_bloc_list(Bloc bloclist){
    Bloc tmp = bloclist;
    while(tmp){
        print_bloc(tmp);
        tmp = tmp->next;
    }
}

void print_page(Bloc bloc){
    mlvalue * tmp = bloc->page;
    size_t cpt = 1;
    while(cpt < bloc->size){
        if(*(tmp+cpt) == 0){
            printf("cpt=%ld, null\n", cpt);
            cpt += 1;
        }else{
            mlvalue obj = Val_ptr(tmp+cpt);
            printf("cpt=%ld, obj=%ld, %s, size=%ld, color=%ld\n", cpt, obj, val_to_str(obj), Size(obj), Color(obj));
            cpt += Size(obj) + 1;
        }
    }
}

void print_heap(Bloc heap){
    Bloc pt = heap;
    while(pt){
        print_bloc(pt);
        print_page(pt);
        pt = pt->next;
    }
}

#endif