# Parser Design and Planning

## Thinking about cases to watch for

* **DONE** What do I do about `()`?
  * I think I keep the same process: make an empty list
  * This is really a null type, which is consistent with Scheme

### Unparseable

* No closing paren
  * Fundamentally: if there's an incomplete parse tree when it exits, that means
    there must be a missing close paren.
  * Occurs if you exit with nonzero depth
* No opening paren

### Edge cases

* **DONE** Starts with close paren
* **DONE** Ends with open paren
* **DONE** There are no tokens at all
  * This seems to work currently. I have no idea why.

## Single quote `'`

* Basic idea:
  * Parse whatever expression follows it
  * Package that into a `(quote expr)` subtree
* I think I implement this in the makeSubtree() function.
  I can implement it as a check: once I finish building a subtree, I can look for a singlequote immediately preceding it.
  * This won't work for cases where I just quote a single token though?
* What if I re-implement the parser using a parseExpression function?

## The Redesign

This should get me in a place where I can implement stuff like single quote more easily.

* So make something like a parseExpression function that recursively calls itself.
* It starts by parsing an expression generally
* I would need to keep track of the current token through multiple call stacks though.

### Parsing a single expression

* Idea: in an expression, you recursively call parseExpression() on each token
  * Inside that call, the current token will be iterated.
  * That call will parse that expression (so `1` or `(2 3)` or `1 (2 3))`)
  * When it returns, we cons that result onto the subtree stack.
  * Now, the current token will be the first token following the expression that was just parsed.
  * So we can call parseExpression() on that token and it will continue.
  * Then, once we hit the end of the current expression, we return the subtree stack (probably reversed) to whatever called it.
* How to distinguish the end of an expression?
  * If the expression isn't a cons, then it's just the one token.
  * If it is a cons, then it's a closing paren.
    * Can I rely on the closing paren?
      Seems like I should, since it should skip over all the closing parens on nexted expressions when it parses those expressions.
      So the first closing paren I encounter should be the correct one for the current expression.
  * Also end of file.
    * Maybe I can use this to check for too many open parens?
    * If I hit the end of a file inside a parseExpression() call, then that means I haven't left an expression yet.
    * This would require that the outermost level of parsing is not a call to parseExpression() but instead its own thing.
    * Similarly, I could use this to check for too many close parens.
      * If there are too many, this would mean that the last token would be a close paren.
      * Or, for that matter, that I see a close paren anywhere in the outer parse function.