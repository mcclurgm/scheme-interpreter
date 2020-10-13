#ifndef _INTERPRETER
#define _INTERPRETER

#include "value.h"

// A frame is a linked list of bindings, and a pointer to another frame.  A
// binding is a variable name (represented as a string), and a pointer to the
// Value it is bound to. Specifically how you implement the list of bindings is
// up to you.
struct Frame {
    Value *bindings;
    struct Frame *parent;
};

typedef struct Frame Frame;

void interpret(Frame *global, Value *tree);
Frame *initialize();

#endif

