; Test load (with 06 and 07)
(define doathing
  (lambda ()
    x))

(display (doathing))
(set! x 5)
"x should be 5 now"

(doathing)