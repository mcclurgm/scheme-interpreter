
(define test
  (lambda (a b)
    (equal? a b)))

(define tree?
  (lambda (bst)
    (cond ((null? bst) '#f)
          ((not (equal? (length bst) 3)) '#f)
          ((not (list? bst)) '#f)
          ((not (list? (car (cdr bst)))) '#f)
          ((not (list? (car (cdr (cdr bst))))) '#f)
          (else '#t))))
          

(define entry
  (lambda (bst)
    (cond ((null? bst) '#f)
          (else (car bst)))))


(define left
  (lambda (bst)
    ; (display "leftresult!!!!!!!!!! ")
    ; (display (tree? bst))
    ; (display " ")
    ; (display bst)
    ; (display " ")
    (cond ((not (tree? bst)) '#f)
          (else (car (cdr bst))))))

(define right
  (lambda (bst)
    (cond ((not (tree? bst)) '#f)
          (else (car (cdr (cdr bst)))))))


(test (entry '()) '#f)
(test (entry '(5 () ())) '5)
(test (left '(5 () )) '#f)
; (test (left '(10 (1 3) (11 13))) '(1 3))
; (test (right '(10 (1 3) (11 13))) '(11 13))
; (test (left '(10 (7 (3 () ()) (8 () ())) (15 () ()))) '(7 (3 () ()) (8 () ())))
(left '(10 (1 3) (11 13))) ; '(1 3)
(right '(10 (1 3) (11 13))) ; '(11 13)
(left '(10 (7 (3 () ()) (8 () ())) (15 () ()))) ; '(7 (3 () ()) (8 () ()))

; (define make-bst
;   (lambda (elt left right)
;     (cond ((not (number? elt)) '#f)
;           ((not (list? left)) '#f)
;           ((not (or (equal? (length left) 3) (equal? (length left) 0))) '#f)
;           ((not (list? right)) '#f)
;           ((not (or (equal? (length right) 3) (equal? (length right) 0))) '#f)
;           (else (list elt left right)))))

; (test (make-bst 5 '(1 2) '()) '#f)
; (test (make-bst 5 '(2 () ()) '()) '(5 (2 () ()) ()))

(define preorder
  (lambda (bst)
    (cond ((null? bst) bst)
          (else 
            (append (list (car bst)) (preorder (left bst)) (preorder (right bst)))))))

; (test (preorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) '(10 7 3 8 15))
(preorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) ; '(10 7 3 8 15)

(define inorder
  (lambda (bst)
    (cond ((null? bst) bst)
          (else (append (inorder (left bst)) (list(car bst)) (inorder (right bst)))))))

; (test (inorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) '(3 7 8 10 15))
(inorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) ; '(3 7 8 10 15)

(define postorder
  (lambda (bst)
    (cond ((null? bst) bst)
          (else (append (postorder (left bst)) (postorder (right bst)) (list(car bst)))))))

; (test (postorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) '(3 8 7 15 10))
(postorder '(10 (7 (3 () ()) (8 () ())) (15 () ()))) ; '(3 8 7 15 10)

(define insert
  (lambda (v bst)
    (cond ((null? bst) (list v '() '()))
          ((< v (car bst)) (list (car bst) (insert v (left bst)) (right bst)))
          ((> v (car bst)) (list (car bst) (left bst) (insert v (right bst))))
          (else bst))))


; (test (insert '2 '(10 (7 (3 () ()) (8 () ())) (15 () ()))) '(10 (7 (3 (2 () ()) ()) (8 () ())) (15 () ())))
(insert '2 '(10 (7 (3 () ()) (8 () ())) (15 () ()))) ; '(10 (7 (3 (2 () ()) ()) (8 () ())) (15 () ()))


(define proper-tree?
  (lambda (bst)
    (cond ((null? bst) '#t)
          ((not (list? bst)) '#f)
          ((not (equal? (length bst) 3)) '#f)
          ((not (in-order? bst)) '#f)
          (else (and (number? (entry bst)) (proper-tree? (left bst)) (proper-tree? (right bst)))))))


(define sorted?
  (lambda (lst)
    (cond ((null? (cdr lst)) '#t)
          ((not(< (car lst) (car (cdr lst)))) '#f)
          (else (sorted? (cdr lst))))))

(define in-order?
  (lambda (bst)
    (cond ((null? bst) '#t)
          (else (sorted? (inorder bst))))))




; (test (proper-tree? '(10 (20 () ()) (13 () ()))) '#f)
(proper-tree? '(10 (20 () ()) (13 () ()))) ; '#f

; (test (proper-tree? '(5 ()())) '#t)
(proper-tree? '(5 ()())) ; '#t

(proper-tree? (insert '2 '(10 (7 (3 () ()) (8 () ())) (15 () ())))) ; #t

(define bst-from-list
  (lambda (lst)
    (cond ((null? lst) '())
          (else (bst-helper (reverse lst))))))

(define bst-helper
  (lambda (lst)
    (cond ((null? lst) '())
          (else (insert (car lst) (bst-helper (cdr lst)))))))


; (test (bst-from-list '(5 4 2 7)) '(5 (4 (2 () ()) ()) (7 () ())))
(bst-from-list '(5 4 2 7)) ; '(5 (4 (2 () ()) ()) (7 () ()))

; (test (proper-tree? (bst-from-list '(5 4 8 3 21 6))) '#t)
