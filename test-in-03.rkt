; List operations

; cons
(cons 'a '()) ; (a)
(cons 'a '(b c)) ; (a b c)
(cons 3 4) ; (3 . 4)
(cons 'a 'c) ; (a . c)

; car
(car '(a)) ; a
(car '(a b c)) ; a
(car (cons 3 4)) ; 3

; cdr
(cdr '(a)) ; ()
(cdr '(a b c)) ; (b c)
(cdr (cons 3 4)) ; 4

; list
(list) ; ()
(list 1 2 3) ; (1 2 3)
(list 3 2 1) ; (3 2 1)

; append
(append '(a b c) '()) ; (a b c)
(append '() '(a b c)) ; (a b c)
(append '(a b) '(c d)) ; (a b c d)
; (append '(a b) 'c) ; (a b . c)
(let ([x (list 'b)])
  (eq? x (cdr (append '(a) x)))) ; #t
; (append '(a) '(b) '(c)) ; (a b c)

; length
(length '()) ; 0
(length '(a b c)) ; 3
;(length '(a b . c)) ; exception
; (length
;   (let ([ls (list 'a 'b)])
;     (set-cdr! (cdr ls) ls)
;     ls)) ; exception
; (length
;   (let ([ls (list 'a 'b)])
;     (set-car! (cdr ls) ls)
;     ls)) ; 2

; list?
(list? '()) ; #t
(list? '(a b c)) ; #t
(list? 'a) ; #f
(list? (cons 3 4)) ; #f
(list? 3) ; #f