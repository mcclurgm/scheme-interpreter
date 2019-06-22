# Planning the interpreter project

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
      * Evaluate the condition
      * If `true`, evaluate and return the second argument
      * Else (`false`), evaluate and return the third argument
      * *Technically*, if the result is not `#f` then it's considered true
        * But Dave's spec requires a `boolean` type so let's not go there.
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
        * Return the result of the last expression
      * `display`
        * Does not return anything
          * Simply prints the result of whatever's in it to the console
            * Does not include a line break
      * `when`
        * If the condition evaluates to `#t`, evaluate all body expressions and return the result of the last
        * If `#f`, don't evaluate anything
          * This should not return anything
      * `unless`
        * If the condition evaluates to `#f`, evaluate all body expressions and return the result of the last
        * If `#t`, don't evaluate anything
          * This should not return anything

## Evaluation errors

* `if`
  * `if` has < 3 arguments
  * Condition in `if` evaluates to not a boolean
* `let`
  * `let` has < 2 arguments (ie, doesn't have a body)
  * `let`: list of bindings isn't a nested list
    * `(let ((x 1)) ...)` is good, not `(let (x 1) ...)`
  * If the first element of a binding pair isn't a symbol / identifier
  * If a binding pair list is not of length 2
  * Can't bind a variable name twice in one `let`
  * Can't bind a variable to something that's bound in the same `let`
    * You can only bind to values from the parent frame, not the new frame
* Unbinded variable
  * Should be handled in searching the frames
* `display`
  * Must have exactly one argument
* `when`, `unless`
  * If it has < 2 arguments
    * 0 arguments: does not have a conditional
    * 1 argument: does not have a body
    * Can basically copy these conditionals from `evalIf`
  * If the condition doesn't result in a boolean

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

## Special Forms

### `lambda`

* Basic format
  * Must have 2 or more elements
  * First element is the params: see next
  * Everything else becomes body, like let

* Params:
  * Has some number of params
  * Can be 0 or more
  * So possible values for the list: `CONS_TYPE` (multiple params) or `NULL_TYPE`
  * Can't have the same name twice
  * Must all be symbols
* Body:
  * Does not need to use all the parameters

* Single variable parameter: eg `(lambda x x)`
* Maybe add bool to Struct?
    * isSpecial?
    * Allows binding single param to body
    * Ex. (define fun (lambda x x)) -> (define 1 2 3) where x -> (1 2 3)


### Define/Lambda

* `Define`
  * Try when first arg is CONS_TYPE vs SYMBOL_TYPE until it breaks
  * Somehow handle bindings and closure type assignment
  * (define (myFn a b c) (+ a b c))

### `let*`

* Allows `(let ((x 3) (x 4)) ...)

### `letrec`

* 2 passes
  * First: binds the variables to a 'junk' (uninitialized) value
  * Second: evaluates the expressions in the `letrec`'s frame and binds
  * In theory, you should be able to identify an uninitialized value but we may not do that here.

### `cond`

* Format:
  * `(cond expr1 expr2 ...)`
  * Can take any number of expressions
    * 0 expressions returns `VOID_TYPE`
    * Also returns `VOID_TYPE` if all are false (unless there's an `else`)
  * Expression format:
    * `(test body)`
      * Test: must evaluate to `#t` or `#f`
        * We can assume this for the test code...
      * Body: any number of expressions, evaluated like begin or let
* `else`:
  * There can't be any other expressions after an `else`

### `and` & `or`
01
* Short circuit
* Empty defaults
  * `and`: `#t`
  * `or`: `#f`
* `and` evaluates to `#t` until it hits a false case, then short circuits.
  `or` evaluates to `#f` until it hits a true case, then short circuits.
* Assuming that all arguments are `BOOL_TYPE`

### Bonus: `load`

* C file handling: this is the code from main that successfully executes
```c
int main() {

    FILE *fp = fopen("test-in-04.rkt","r");
    if (fp == NULL) {
        printf("Can't open input file in.list!\n");
        exit(1);
    }

    Value *list = tokenize(fp);
    Value *tree = parse(list);
    interpret(tree);

    tfree();
    fclose(fp);
    return 0;
}
```

## Primitives

### `=`

* Like `equal?` for numbers
* Requires numbers, exit with error if you get other input
* Takes at least 1 number
* Returns true if all of the arguments are equal
* Returns false if any of them is not equal to the rest

### `append`

* Can take any number of arguments
  * 0: return an empty list
  * 1: return the argument
  * 2+: add each one on to the appended versions of the next
    * From Dybvig: 
      > append returns a new list consisting of the elements of the first list followed by the elements of the second list, the elements of the third list, and so on. The new list is made from new pairs for all arguments but the last; the last (which need not be a list) is merely placed at the end of the new structure. append may be defined without error checks as follows. 
* Reverse the list of arguments, so I can iterate through it in the right order.
* Then, I can just call append() one after the next
  * `current = car(args);`
  * Iterate current
  * `current = append(car(args), current);`