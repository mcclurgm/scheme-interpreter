; Test boolean functions: cond, and, or, if
(cond
    (#f 1)
    (#t 2)
    (else "you should never see me"))

(cond)

(cond
    ((equal? 1 2) 2)
    ((equal? 1 1) 1))

(define avariable 1)

(define (ohno x)
    (set! avariable x)
    #t)

(and #t (ohno 2) #f)
avariable
(and #t #f (ohno 3))
avariable

(or (ohno 5) #t #f)
avariable
(or #t #f (ohno 6))
avariable

(and)
(or)