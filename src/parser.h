#ifndef _PARSER
#define _PARSER

#include "value.h"

// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens);


// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
void printTree(Value *tree);

// Prints a value (not a tree) to the screen in a readable fasion.
// Designed as a helper function to printTree, but can have uses on its own.
void printValue(Value *val);


#endif
