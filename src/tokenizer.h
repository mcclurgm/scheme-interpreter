#ifndef _TOKENIZER
#define _TOKENIZER

#include "value.h"
#include "linkedlist.h"
#include "talloc.h"

// Read all of the input from stdin, and return a linked list consisting of the
// tokens.
Value *tokenize();

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list);

#endif
