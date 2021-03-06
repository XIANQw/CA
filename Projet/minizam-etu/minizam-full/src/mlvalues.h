#ifndef _MLVALUES_H
#define _MLVALUES_H

#include <stdint.h>
#include <stdlib.h>
#include "config.h"

typedef int64_t mlvalue;
typedef uint64_t header_t;
typedef enum { WHITE, GRAY, BLACK } color_t;
typedef enum { YOUNG, OLD } gene_t;
typedef enum { ENV_T, CLOSURE_T, BLOCK_T, FWD_PTR_T } tag_t;

/* If a mlvalue ends with 1, it's an integer, otherwise it's a pointer. */
#define Is_long(v)  (((v) & 1) != 0)
#define Is_block(v) (((v) & 1) == 0)
#define Val_long(v) (((v) << 1) + 1)
#define Long_val(v) (((uint64_t)(v)) >> 1)

#define Val_ptr(p) ((mlvalue)(p))
#define Ptr_val(v) ((mlvalue*)(v))
#define Val_hd(hd) ((mlvalue)(hd))

#ifdef GENERAL
/* Structure of the header:
     +--------+-------+----------+---+
     | size   | color |generation|tag|
     +--------+-------+----------+---+
bits  63    10 9      8          7   0
*/
#define Tag_hd(hd)   ((hd) & 127)
#define Gene_hd(hd)  (((hd) >> 7) & 1)
#define Gene(v)  Gene_hd(Hd_val(v))
#define Make_header(size,color, gene, tag) \
  ((header_t)(((size) << 10) | (((color) & 3) << 8) | (((gene) & 1) << 7) | ((tag) & 127)))

#endif



#if (defined MARK_n_SWEEP) || (defined STOP_n_COPY)
/* Structure of the header:
     +--------+-------+---+
     | size   | color |tag|
     +--------+-------+---+
bits  63    10 9      8   0
*/

#define Tag_hd(hd)   ((hd) & 0xFF)
#define Make_header(size,color,tag) \
  ((header_t)(((size) << 10) | (((color) & 3) << 8) | ((tag) & 0xFF)))
#endif


#define Size_hd(hd)  ((hd) >> 10)
#define Color_hd(hd) (((hd) >> 8) & 3)


#define Hd_val(v) (((header_t*)(v))[-1])
#define Field(v,n) (Ptr_val(v)[n])
#define Field0(v) Field(v,0)
#define Field1(v) Field(v,1)
#define Size(v) Size_hd(Hd_val(v))
#define Color(v) Color_hd(Hd_val(v))
#define Tag(v)  Tag_hd(Hd_val(v))

#define WHITE 0
#define GRAY 1
#define BLACK 2


#define Addr_closure(c) Long_val(Field0(c))
#define Env_closure(c)  Field1(c)
#define New_ptr(block) Field(block, Size(block))

#if (defined STOP_n_COPY) || (defined GENERAL) 
#define ADD_SIZE 2
#endif

#ifdef MARK_n_SWEEP
#define ADD_SIZE 1
#endif


/* Structure of block:
     +--------+------+--------+
     | header | data | newptr |
     +--------+------+--------+
index-1       0      size   size+1
*/


#ifdef GENERAL
#define Make_empty_block(accu, tag) \
        accu = Val_ptr(caml_alloc(ADD_SIZE)); \
        Field(accu, 0) = Make_header(0, WHITE, YOUNG, tag); \
        accu = Val_ptr(Ptr_val(accu)+1)

#define Make_block(accu, size, tag) \
        accu = Val_ptr(caml_alloc(size + ADD_SIZE)); \
        Field(accu, 0) = Make_header(size, WHITE, YOUNG, tag);\
        accu = Val_ptr(Ptr_val(accu)+1)

#define Make_closure(accu, addr, env) \
        accu = Val_ptr(caml_alloc(2 + ADD_SIZE)); \
        Field(accu, 0) = Make_header(2, WHITE, YOUNG, CLOSURE_T); \
        Field(accu, 1) = Val_long(addr); \
        Field(accu, 2) = env; \
        accu = Val_ptr(Ptr_val(accu)+1)

#endif

#if (defined MARK_n_SWEEP) || (defined STOP_n_COPY)

#define Make_empty_block(accu, tag) \
        accu = Val_ptr(caml_alloc(ADD_SIZE)); \
        Field(accu, 0) = Make_header(0, WHITE, tag); \
        accu = Val_ptr(Ptr_val(accu)+1)

#define Make_block(accu, size, tag) \
        accu = Val_ptr(caml_alloc(size + ADD_SIZE)); \
        Field(accu, 0) = Make_header(size, WHITE, tag);\
        accu = Val_ptr(Ptr_val(accu)+1)

#define Make_closure(accu, addr, env) \
        accu = Val_ptr(caml_alloc(2 + ADD_SIZE)); \
        Field(accu, 0) = Make_header(2, WHITE, CLOSURE_T); \
        Field(accu, 1) = Val_long(addr); \
        Field(accu, 2) = env; \
        accu = Val_ptr(Ptr_val(accu)+1)

#endif



#define Make_empty_env(env) Make_empty_block(env, ENV_T)

#define Make_env(env, size) Make_block(env, size, ENV_T)


mlvalue make_list(int n);


#define Unit Val_long(0)


void print_val(mlvalue val);
char* val_to_str(mlvalue val);

/* A bytecode is represented as a uint64_t. */
typedef uint64_t code_t;

int print_instr(code_t* prog, int pc);
void print_prog(code_t* code);


#endif /* _MLVALUES_H */
