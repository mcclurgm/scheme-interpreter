# Planning the evaluation project

* When do we create a new frame? (When we create new variable names)
  * Currently `let`

* Nesting things:
  * `(let (x 1) (let (y x)))`


## Implementation

* Looking up bindings:
  * Go up environments in order
  * Start at innermost environment, go up successively
  * If nothing (including global as ending case) has it, error
    * Should be already good because global will be the last frame we can go to


## `eval`

* Cons-type
  * Special forms: check for the keywords we know
    * `if`
      * Call function `evalIf`
        * Arguments: everything after the `if` token
        * Evaluate each expression (using `eval`, probably) 
    * `let`
      * Evaluate binding pairs: arbitrary number of expressions
        * Each is a pair of an identifier name and a value
          * Identifier must be a pure symbol, not an expression
          * Value can be an expression that we then have to `eval`
            * Use the parent frame (where `let` is called in: denoted `e`)
        * From this, create a new frame `f`
      * Evaluate each expression in the body
        * Loop through and call `eval` on each?
        * Using the new frame from the let: `f`

## Evaluation errors

* `if`
  * `if` has < 3 arguments
  * Condition in `if` evaluates to not a boolean
* `let`:
  * `let` has < 2 arguments
  * `let`: list of bindings isn't a nested list
    * `(let ((x 1)) ...)` is good, not `(let (x 1) ...)`
  * If the first element of a binding pair isn't a symbol / identifier
  * If a binding pair list is not of length 2
* Unbinded variable
  * Should be handled in searching the frames

## Frame handling

* Each `Frame` has a pointer to its parent and the bindings (defined in `interpreter.h`)
* If the parent is null, then we're at the global and any variable lookup has failed
  * How do we implement this null??? A null pointer or something
    * `parent = NULL;`
    * This will break the idea of being able to just `assert(some pointer != NULL)` for dealing with frames only

### Bindings

* This is a linked list of binding pointers
* Car of each element is a binding pair
* A binding pair is a cons cell
  * `car` is the variable name (`SYMBOL_TYPE` `Value`)
  * `cdr` is the value it's bound to (arbitrary type as long as it's a valid value)
  * We can make a `makeBinding(Value *name, Value *val)`
    * `name` is a symbol type and you should just be able to pass it directly from the tree