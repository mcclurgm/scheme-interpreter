(define doathing
  (lambda ()
    x))

(display (doathing))
(set! x 5)
"x should be 5 now"

(doathing)