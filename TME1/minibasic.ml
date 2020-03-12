let _ = 
  let lexbuf =Lexing.from_channel stdin in
  let ast = Parser.s Lexer.tokens lexbuf in 
  Eval.eval ast
