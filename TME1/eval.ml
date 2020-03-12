open Ast;;

let rec eval (ast: expr) : unit = 
  match ast with
  | INT(i) -> Printf.printf "%d " i;
  | ADD(e1,e2) -> 
  Printf.printf "%d" e1;
  print_string "+"; 
  eval e2;
  | SUB(e1, e2) -> 
  Printf.printf "%d" e1;
  print_string "-"; 
  eval e2;
  | MUL(e1, e2)->
  Printf.printf "%d" e1;
  print_string "*"; 
  eval e2;
  |DIV(e1, e2)->
  Printf.printf "%d" e1;
  print_string "/"; 
  eval e2;
;;