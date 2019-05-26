(define not
    (lambda (bool x)
        (if bool #f #t)))

(not #t 1)