(define gen-list
  (lambda (start stop)
    (if (> start stop)
        '()
        (cons start (gen-list (+ start 1) stop)))))

;(check-equal? (gen-list 1 3)
;              '(1 2 3))
;(check-equal? (gen-list 1 1)
;              '(1))
;(check-equal? (gen-list 2 1)
;              '())

(define any-sum-each?
  (lambda (a lst val)
    (if (null? lst)
        #f
        (if (equal? (+ a (car lst)) val) #t
        (any-sum-each? a (cdr lst) val)))))

(define any-sum?
 (lambda (lst val)
   (cond ((< (length lst) 2) #f)
         ((any-sum-each? (car lst) (cdr lst) val) #t) ; Check (car lst) added with all other options in (cdr val)
         (else (any-sum? (cdr lst) val))))) ; Try using the next element as the one to add

;(check-equal? (any-sum? (gen-list 1 10) 1000) #f)
;(check-equal? (any-sum? '(1 2 3) 3) #t)

(define pair-sum?
  (lambda (lst val)
    (cond ((< (length lst) 2) #f)
          ((= (+ (car lst) (car (cdr lst))) val) #t)
          (else (pair-sum? (cdr lst) val)))))
;(check-equal? (pair-sum? (gen-list 1 10) 1000) #f)
;(check-equal? (pair-sum? '(1 2 3) 3) #t)

(define gen-lazy-list
  (lambda (start stop)
    (if (> start stop)
        #f
        (cons start
            (lambda () (gen-lazy-list (+ start 1) stop))))))

(define pair-sum-lazy?
  (lambda (lazy val)
    (cond ((equal? ((cdr lazy)) #f) #f) ; Return false if you're at the last element of the list.
          ((equal? (+ (car lazy) (car ((cdr lazy)))) val) #t) ; Return true if a pair matches!
          (else (pair-sum-lazy? ((cdr lazy)) val)))))

;(check-equal? (pair-sum-lazy? (gen-lazy-list 1 10) 1000) #f)
;(check-equal? (pair-sum-lazy? (gen-lazy-list 1 3) 3) #t)

(define any-sum-each-lazy?
  (lambda (a lst val)
    (cond ((equal? (+ a (car lst)) val) #t)
          ((equal? ((cdr lst)) #f) #f) ; Return false if you're at the last element of the list, since there are no more options.
          (else (any-sum-each-lazy? a ((cdr lst)) val))))) ; Try next...

(define any-sum-lazy?
 (lambda (lst val)
   (cond ((equal? ((cdr lst)) #f) #f) ; Return false if you're at the last element of the list.
         ((any-sum-each-lazy? (car lst) ((cdr lst)) val) #t) ; Check (car lst) added with all other options in (cdr val)
         (else (any-sum-lazy? ((cdr lst)) val))))) ; Try using the next element as the one to add

(any-sum-lazy? (gen-lazy-list 1 10) 1000)
(any-sum-lazy? (gen-lazy-list 1 3) 3)

(define make-lazy
  (lambda (lst)
    (if (null? lst)
        #f
        (cons (car lst)
              (lambda ()
                (make-lazy (cdr lst)))))))