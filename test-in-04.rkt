(+ 1 (+ 1 2))

(null? (quote ()))

(car (quote (1 2 3)))

(cdr (quote (1 2)))

(cons 1 (quote(2)))

(list 1 2)

(equal? 1 1)

(define a 3)
(define b a)

(eq? a b)
(eq? a a)
(equal? a b)

(append () (list 1 2))