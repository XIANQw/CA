type expr = 
  |INT of int
  |STRING of string
  |BOOL of bool
  |ADD of int * expr
  |MUL of int * expr
  |DIV of int * expr
  |SUB of int * expr
  |LT of int * expr
  |EQ of int * expr
  |RT of int * expr


