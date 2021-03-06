#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "domain_state.h"
#include "interp.h"
#include "instruct.h"
#include "alloc.h"
#include "primitives.h"



// #define DEBUG
#define env Caml_state->env


mlvalue caml_interprete(code_t* prog) {

  mlvalue accu = Val_long(0);
  // mlvalue env = Make_empty_env();
  Make_empty_env(env);

  register unsigned int pc = 0;
  unsigned int extra_args = 0;
  unsigned int trap_sp = 0;

  while(1) {

#ifdef DEBUG
      // printf("pc=%d accu=%s heap=%d Sp=%d extra_args=%d trap_sp=%d\n",
      //        pc, val_to_str(accu), Caml_state->alloc_ptr, Sp, extra_args, trap_sp);
      printf("pc=%d heap=%d Sp=%d extra_args=%d trap_sp=%d ",
             pc, Caml_state->alloc_ptr, Sp, extra_args, trap_sp);
      if(Is_long(accu)) printf("accu=%s\n", val_to_str(accu));
      else printf("Tag(accu)=%ld\n", Tag(accu));
      /*
      printf("pc=%d  accu=%s heap=%d Sp=%d extra_args=%d trap_sp=%d Stack=[",
             pc, val_to_str(accu), Caml_state->alloc_ptr, Sp, extra_args, trap_sp);

      if (Sp > 0) {
        printf("%s", val_to_str(Stack[Sp-1]));
      }
      for (int i = Sp-2; i >= 0; i--) {
        printf(";%s", val_to_str(Stack[i]));
      }
      printf("]  env=%s\n", val_to_str(env));
      */
      print_instr(prog, pc);
#endif
    switch (prog[pc++]) {
    case CONST:
      accu = Val_long(prog[pc++]);
      break;

    case PRIM:
      switch (prog[pc++]) {
      case ADD:   accu = ml_add(accu, POP_STACK()); break;
      case SUB:   accu = ml_sub(accu, POP_STACK()); break;
      case DIV:   accu = ml_div(accu, POP_STACK()); break;
      case MUL:   accu = ml_mul(accu, POP_STACK()); break;
      case OR:    accu = ml_or(accu, POP_STACK()); break;
      case AND:   accu = ml_and(accu, POP_STACK()); break;
      case NOT:   accu = ml_not(accu); break;
      case NE:    accu = ml_ne(accu, POP_STACK()); break;
      case EQ:    accu = ml_eq(accu, POP_STACK()); break;
      case LT:    accu = ml_lt(accu, POP_STACK()); break;
      case LE:    accu = ml_le(accu, POP_STACK()); break;
      case GT:    accu = ml_gt(accu, POP_STACK()); break;
      case GE:    accu = ml_ge(accu, POP_STACK()); break;
      case PRINT: accu = ml_print(accu); break;
      }
      break;

    case BRANCH:
      pc = prog[pc];
      break;

    case BRANCHIFNOT:
      if (Long_val(accu) == 0) {
        pc = prog[pc];
      } else {
        pc++;
      }
      break;

    case PUSH:
      PUSH_STACK(accu);
      break;

    case POP:
      POP_STACK();
      break;

    case ACC:
      accu = Stack[Sp-prog[pc++]-1];
      break;

    case ENVACC:
      accu = Field(env, prog[pc++]);
      break;

    case APPLY: {
      // recuperer le nombre des arguments
      uint64_t n = prog[pc++];
      // laisser 3 cases pour env, pc, extra_args
      Sp += 3;
      for (uint64_t i = 0; i < n; i++) {
        Stack[Sp - i - 1] = Stack[Sp - i - 3 - 1]; // tous les argus montent 3 cases
      }
      Stack[Sp - n - 3] = env;
      Stack[Sp - n - 2] = Val_long(pc);
      Stack[Sp - n - 1] = Val_long(extra_args);
      pc = Addr_closure(accu);
      env = Env_closure(accu);
      extra_args = n-1;
      break;
    }

    case APPTERM: {
      uint64_t n = prog[pc++];
      uint64_t m = prog[pc++];
      int delta = m - n;
      if(delta > 0){
        Sp -= delta;
        for (int i = n - 1; i >= 0; i--) {
          Stack[Sp - i - 1] = Stack[Sp - i - 1 + delta];
        }
      }
      pc = Addr_closure(accu);
      env = Env_closure(accu);
      extra_args += n-1;
      break;
    }

    case RETURN: {
      uint64_t n = prog[pc++];
      for (uint64_t i = 0; i < n; i++) {
        POP_STACK();
      }
      if (extra_args == 0) {
        extra_args = Long_val(POP_STACK());
        pc  = Long_val(POP_STACK());
        env = POP_STACK();
      } else {
        extra_args--;
        pc = Addr_closure(accu);
        env = Env_closure(accu);
      }
      break;
    }

    case RESTART: {
      unsigned int n = Size(env);
      for (unsigned int i = n-1; i > 0; i--) {
        PUSH_STACK(Field(env,i));
      }
      env = Field(env,0);
      extra_args += n-1;
      break;
    }

    case GRAB: {
      uint64_t n = prog[pc++];
      if (extra_args >= n) {
        extra_args -= n;
      } else {
        // mlvalue closure_env = Make_env(extra_args + 2);
        mlvalue closure_env;
        Make_env(closure_env, extra_args + 2);
        Field(closure_env,0) = env;
        for (unsigned int i = 0; i <= extra_args; i++) {
          Field(closure_env,i+1) = POP_STACK();
        }
        Make_closure(accu, pc-3, closure_env);
        extra_args = Long_val(POP_STACK());
        pc  = Long_val(POP_STACK());
        env = POP_STACK();
      }
      break;
    }
  
    case CLOSURE: {
      uint64_t addr = prog[pc++];
      uint64_t n = prog[pc++];
      if (n > 0) {
        PUSH_STACK(accu);
      }
      Make_closure(accu, addr, env);
      mlvalue closure_env;
      Make_env(closure_env, n+1);
      // mlvalue closure_env = Make_env(n+1);
      Field(closure_env,0) = Val_long(addr);
      for (uint64_t i = 0; i < n; i++) {
        Field(closure_env,i+1) = POP_STACK();
      }
      // accu = make_closure(addr,closure_env);
      Field(accu,1) = closure_env;
      break;
    }

    case CLOSUREREC: {
      uint64_t addr = prog[pc++];
      uint64_t n = prog[pc++];
      if (n > 0) {
        PUSH_STACK(accu);
      }
      // mlvalue closure_env = Make_env(n+1);
      Make_closure(accu, addr, env);
      mlvalue closure_env;
      Make_env(closure_env, n+1);
      Field(closure_env,0) = Val_long(addr);
      for (uint64_t i = 0; i < n; i++) {
        Field(closure_env,i+1) = POP_STACK();
      }
      // accu = make_closure(addr,closure_env);
      Field(accu, 1) = closure_env;
      PUSH_STACK(accu);
      break;
    }

    case OFFSETCLOSURE: {
      Make_closure(accu, Long_val(Field(env,0)), env);
      break;
    }

    case MAKEBLOCK: {
      uint64_t n = prog[pc++];
      mlvalue tmp = accu;
      Make_block(accu, n, BLOCK_T);
      if (n > 0) {
        Field(accu,0) = tmp;
        for (unsigned int i = 1; i < n; i++) {
          Field(accu, i) = POP_STACK();
        }
      }
      break;
    }

    case GETFIELD: {
      uint64_t n = prog[pc++];
      accu = Field(accu, n);
      break;
    }

    case VECTLENGTH: {
      accu = Val_long(Size(accu));
      break;
    }

    case GETVECTITEM: {
      uint64_t n = Long_val(POP_STACK());
      accu = Field(accu, n);
      break;
    }

    case SETFIELD: {
      uint64_t n = prog[pc++];
      Field(accu, n) = POP_STACK();
      break;
    }

    case SETVECTITEM: {
      uint64_t n = Long_val(POP_STACK());
      mlvalue v = POP_STACK();
      Field(accu, n) = v;
      accu = Unit;
      break;
    }

    case ASSIGN: {
      uint64_t n = prog[pc++];
      Stack[Sp-n-1] = accu;
      accu = Unit;
      break;
    }

    case PUSHTRAP: {
      uint64_t addr = prog[pc++];
      PUSH_STACK(Val_long(extra_args));
      PUSH_STACK(env);
      PUSH_STACK(Val_long(trap_sp));
      PUSH_STACK(Val_long(addr));
      trap_sp = Sp;
      break;
    }

    case POPTRAP: {
      POP_STACK(); // popping pc
      trap_sp = Long_val(POP_STACK());
      POP_STACK(); // popping env
      POP_STACK(); // popping extra_args
      break;
    }

    case RAISE: {
      if (trap_sp == 0) {
        fprintf(stderr, "Uncaught exception: %s\n", val_to_str(accu));
        exit(EXIT_FAILURE);
      } else {
        Sp = trap_sp;
        pc = Long_val(POP_STACK());
        trap_sp = Long_val(POP_STACK());
        env = POP_STACK();
        extra_args = Long_val(POP_STACK());
        break;
      }
    }

    case STOP:
      return accu;

    default:
      fprintf(stderr, "Unkown bytecode: %lu at offset %d\n", prog[pc-1], pc-1);
      exit(EXIT_FAILURE);
    }
  }

}
