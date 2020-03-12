let rec f n =
  if n > 10 then 0
  else (1 + (f (n + 3)))

let _ = f 3