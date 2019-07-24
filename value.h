#ifndef _VALUE
#define _VALUE

#include <stdbool.h>

typedef enum {INT_TYPE,DOUBLE_TYPE,STR_TYPE,CONS_TYPE,NULL_TYPE,PTR_TYPE,
              OPEN_TYPE,CLOSE_TYPE,BOOL_TYPE,SYMBOL_TYPE, 
              DOT_TYPE, OPEN_BRACKET_TYPE, CLOSE_BRACKET_TYPE, QUOTE_TYPE,
              VOID_TYPE, CLOSURE_TYPE, PRIMITIVE_TYPE, UNINITIALIZED} valueType;

struct Value {
    valueType type;
    bool marked;
    union {
        int i;
        double d;
        char *s;
        void *p;
        struct ConsCell {
            struct Value *car;
            struct Value *cdr;
        } c;
        struct Closure {
            struct Value *paramNames;
            struct Value *functionCode;
            struct Frame *frame;
        } cl;
		struct Value *(*pf)(struct Value *);
    };
};

typedef struct Value Value;

// Allocates a Value struct and initializes its marked attribute to false.
Value *makeValue();

// Create a new NULL_TYPE Value.
Value *makeNull();

// Create a new VOID_TYPE Value.
Value *makeVoid();

// Create a new INT_TYPE Value.
Value *makeInt(int val);

// Create a new DOUBLE_TYPE Value.
Value *makeDouble(double val);

// Create a new BOOL_TYPE Value.
Value *makeBool(bool val);

// Create a new STR_TYPE Value.
Value *makeString(bool val);

// Create a new SYMBOL_TYPE Value.
Value *makeSymbol(char *val);

bool isList(Value *list);

bool isNumber(Value *value);

#endif
