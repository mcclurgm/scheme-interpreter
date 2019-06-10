THE GREAT INTERPRETER PROJECT

We implemented load for our "bonus".

It takes in exactly one argument, the relative path to a file on the disk.
It tokenizes, parses, and executes the code in that file.

It executes everything in the global frame, as if the code it loads were
top-level in the original file. It ignores any nested frames, like from
a let expression.
It modifies the global frame in the same way too: both define and set! will
modify variables bound in the global frame. It does not read or change any
variables bound in a child frame (again, like a let frame).

It returns the value of the last statement in the file, kind of like begin.
It will output any calls to display, but otherwise doesn't return any other
data to the original program.

Our test file is test-in-06.rkt, which uses test-in-07.rkt and test-in-08.rkt.
It should output "3575". All of these are printed from test-in-08.rkt, which
is called from test-in-07. The 3 is the original value of the variable x,
5 is after set! changes it to 5, 7 is the value printed from a let frame in 
test-in-07 that loads the code (to verify that it doesn't change the let's
binding), and the last 5 checks that the new value of the global x persists
to the original program.