(define a (lambda (x) x))

(define x 3)
(display "a:")
(display a)
(display "x:")
(display x)

(define (f x y) x)
(display "f:")
(display f)
(f 1 2)
(define (f) 2)
(f)