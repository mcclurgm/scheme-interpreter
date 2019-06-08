; (let* ((x 3) (x 4) (y x)) y)
; (let* ((x 3) (x 4) (y (+ x 1))) y)

; (define x 5)
; ((let ((x 3)) (lambda () (set! x 4) x))) ;4
; x ;5

; (let* ((x 1) (y x)) y)
; (begin (define a 1) (set! a 2) a)
; (begin x)

(letrec ((x 1) (y 3)) y)
(letrec ((sum (lambda (x)
                (if (null? x)
                    0
                    (+ (car x) (sum (cdr x)))))))
  (sum (quote (1 2 3 4 5))))