; Tests tokenizer
(+ x2 ( + ( quote x ) "foo;; still a string" 323) ;; now it's a comment!
#t #f +1 -2. +.123 abc.123 + - !$$%&*/:+-.<=>?~_^ .
1""1 .1 a'1 [b]
