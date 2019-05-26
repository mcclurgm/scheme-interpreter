(define a 3)

(define not
    (lambda (bool x)
        (if bool #f #t)))

(not #t 1)