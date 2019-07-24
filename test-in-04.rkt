; Test math operations

; number?
(number? 1)
(number? 2.0)
(number? '())

; Multiplication
(*)
(* 2)
(* 2 3)
(* 2 3 4)
(* 2.0 3.0)
(* 1.0 2)

; Subtraction
(- 2 3)
(- 2 3 4)
(- 2.0 3.0)
(- 1.0 2)
(- 4 2 -4)

; Division
(/ 1 2)
(/ 2 1)
(/ 1.0 2)
(/ 2.0 1)
(/ 2.0 2.0)
(/ 5 2)

; Less than
(< 1 2)
(< 3 2)
(< 1.0 2)
(< 2 1.0)
(< 3.0 5.0)

; Greater than
(> 1 2)
(> 3 2)
(> 1.0 2)
(> 2 1.0)
(> 3.0 5.0)

; Modulo
(modulo 1 3) ;1
(modulo 3 1) ;0
(modulo 2 3) ;2
(modulo 4300 17) ;16

; = (number equivalence)
(= 1 1)
(= 1 2)
(= 1.0 1)
(= 1 2.0)
(= 1.0 2.0)
(= 1.0 1 1 1 1 1 1.0000000000000 2)
(= 1)
(= 1 ((lambda () 1)))
(define x 1)
(= x 1)

(let* ((x 3) (y x)) y)