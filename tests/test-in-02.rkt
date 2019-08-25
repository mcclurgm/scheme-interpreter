; Test boolean functions: cond, and, or, if, not, equal, eq

; cond
(cond
    (#f 1)
    (#t 2)
    (else "you should never see me")) ; 2
(cond) ; void
(cond
    ((equal? 1 2) 2)
    ((equal? 1 1) 1)) ; 1
(cond (1 2)) ; 2
(cond (#f 1)
      (else "else")) ; "else"

; if
(if #t
    1
    2) ; 1
(if #f
    1
    2) ; 2
(if "stuff"
    1
    2) ; 1

(define income-tax
  (lambda (income)
    (cond
      [(<= income 10000) (* income .05)]
      [(<= income 20000) (+ (* (- income 10000) .08) 500.00)]
      [(<= income 30000) (+ (* (- income 20000) .13) 1300.00)]
      [else (+ (* (- income 30000) .21) 2600.00)])))

; (income-tax 5000) ; 250.0
; (income-tax 15000) ; 900.0
; (income-tax 25000) ; 1950.0
; (income-tax 50000) ; 6800.0

; and
(and 1 2) ; 2
(and 1 2 #f) ; #f
; use side effects to test short circuit
(define avariable 1)
(define (ohno x)
    (set! avariable x)
    #t)
(and #t (ohno 2) #f) ; #f
avariable ; 2
(and #t #f (ohno 3)) ; #f
avariable ; 2
(and) ; #t

; or
(or 1 2) ; 1
(or 1 2 #f) ; 1
; use side effects to test short circuit
(or (ohno 5) #t #f) ; #t
avariable ; 5
(or #t #f (ohno 6)) ; #t
avariable ; 5
(or) ; #f
; output line 16

; not
(not #t) ; #f
(not #f) ; #t
(not 1)
(not "false") ; #f

; equal
(equal? 1 1)
(equal? 'a 3) ; #f
(equal? #t 't) ; #f
(equal? "abc" 'abc) ; #f
(equal? "hi" '(hi)) ; #f
(equal? #f '()) ; #f
(equal? 3.4 53344) ; #f
(equal? 3 3.0) ; #f
(equal? 3.4 (+ 3.0 .4)) ; #t
(let ([x (* 12345678987654321 2)])
  (equal? x x)) ; #t
(equal? #t #t) ; #t
(equal? #f #f) ; #t
(equal? #t #f) ; #f
(equal? (null? '()) #t) ; #t
(equal? (null? '(a)) #f) ; #t

(define a 3)
(define b a)
(equal? a b)

; eq
(eq? a b)
(eq? a a)
