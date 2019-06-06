(let* ((x 3) (x 4) (y x)) y)
(let* ((x 3) (x 4) (y (+ x 1))) y)

(define x 5)
((let ((x 3)) (lambda () (set! x 4) x)))
x