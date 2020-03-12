%{
open Ast
%}

/*  + - * / */
%token ADD SUB MUL DIV
/* ( ) [ ] { } */
%token LP RP LC RC LA RA
/* relation */
%token LT GT EQ 
%token<int> NUM
%token<string> IDENT
%token EOF

%start<Ast.expr> s 

%%

s: e=expr EOF {e}

expr : 
e=NUM {INT(e)}
|e=IDENT {STRING(e)}
|e1=NUM op=ADD e2=expr {ADD(e1, e2)}
|e1=NUM op=SUB e2=expr {SUB(e1, e2)}
|e1=NUM op=MUL e2=expr {MUL(e1, e2)}
|e1=NUM op=DIV e2=expr {DIV(e1, e2)}
|e1=NUM op=LT e2=expr {LT(e1, e2)}
|e1=NUM op=EQ e2=expr {EQ(e1, e2)}
|e1=NUM op=GT e2=expr {GT(e1, e2)}
%%

