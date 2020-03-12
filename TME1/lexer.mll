{
  open Parser
}
let alpha  = ['a'-'z']*
let eol = '\n'

rule tokens  = parse
  |['0'-'9']+ as i {NUM(int_of_string i)}
  | '+' {ADD}
  | '*' {MUL}
  | '-' {SUB}
  | '/' {DIV}
  | '(' {LP} 
  | ')' {RP}
  | '<' {LT}
  | '=' {EQ}
  | '>' {GT}
  | alpha as c {IDENT(c)}
  | eof {EOF}



  