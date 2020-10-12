# Planning the interpreter project

* When do we create a new frame? (When we create new variable names)
  * Currently `let`

* Nesting things:
  * `(let (x 1) (let (y x)))`


## Core

### REPL

* Step 1: get a loop in C to read from the command line
  * This ought to be a pain.
  * Here are some sources with info:
    * [Dave's project description](https://www.cs.carleton.edu/faculty/dmusicant/CS251-00-s19/interpreter/interp9complete.html), which describes an issue about unnecessary prompts
    * [Ohio State on interactive C input](https://www.asc.ohio-state.edu/physics/ntg/780/handouts/interactive_input_in_C.pdf)
* Step 2: run that through evaluation
  * Current method in `main`:
    ```C
    Value *list = tokenize(stdin);
    Value *tree = parse(list);
    interpret(tree);
    ```
  * Should probably make this a function that I can then call from the REPL implementation and `main`.
* Step 3: make data persistent
  * I need to keep all the bindings around between successive calls to the evaluator, since that's how the REPL works.

**Actually,** my implementation of `tokenize` reads directly from an input stream (only I pass it a file pointer instead of `stdin`). If I can rearrange that to accept `stdin` (and the other formatting that comes with it), then it should be a fairly simple change. 

New steps:
1. Accept arguments for filename
2. Run `tokenize`, `parse`, and `interpret` in a loop from `main`.
3. Adjust `tokenize` to accept the usual command-line trappings. Ctrl-D on its own line should exit. (Dunno what happens if I just add it to a random line.) Deal with EOL's, and I shouldn't be getting EOF.
4. Make data persistent

### `eval`

* Cons-type
  * Special forms: check for the keywords we know
    * `if`
      * Call function `evalIf`
        * Arguments: everything after the `if` token
        * Evaluate each expression (using `eval`, probably)
      * Evaluate the condition
      * If true, evaluate and return the second argument
      * Else (`#f`), evaluate and return the third argument
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
        * If the condition evaluates to true, evaluate all body expressions and return the result of the last
        * If `#f`, don't evaluate anything
          * This should not return anything
      * `unless`
        * If the condition evaluates to `#f`, evaluate all body expressions and return the result of the last
        * If true, don't evaluate anything
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
* Looking up bindings:
  * Go up environments in order
  * Start at innermost environment, go up successively
  * If nothing (including global as ending case) has it, error
    * Should be already good because global will be the last frame we can go to

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
      * Test: evaluate, get its truth value
      * Body: any number of expressions, evaluated like begin or let
      * If test is true, evaluate body
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
* Following typical Boolean logic, anything except the literal `#f` is considered true.
  * It returns the value that determines the expression's truth value. So instead of always returning `#t` or `#f`, it can return any object
    * This works with short-circuiting.
    * `or`: returns the first true parameter (which initializes the short-circuit), otherwise `#f`
      * Example: `(or #f 1 2 3 #f) -> 1`
      * `(or #f) -> #f`
    * `and` follows similar logic, although it's expressed differently. It returns the value that short-circuits it (which will always be `#f`, since that's the only false value), otherwise the last value before a short-circuit exit.
      * Since the only way to return true is for everything to evaluate to true (ie, no short-circuiting), the true value returned is the last parameter.
      * Any false value (`#f`) returns `#f`.
      * `(and 1 2 #f 3) -> #f`
      * `(and 1 2 3) -> 3`

### `load`

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