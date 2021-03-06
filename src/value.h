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

// Allocates a Value struct, sets its type, and initializes it as unmarked.
Value *makeValue(valueType type);

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
Value *makeString(char * val);

// Create a new SYMBOL_TYPE Value.
Value *makeSymbol(char *val);

// Note that there is not a makeCons() function. This is intentional, since the
// cons() function should be used instead. It is both easier and ensures that
// cons cells are Scheme-valid, with values in both car and cdr.

// Check that the value is a cons type (ie cons cell).
bool isCons(Value *value);

// Check that the value is the head of a proper list structure.
//TODO Note, this does not check for circular lists.
bool isProperList(Value *value);

// Check that the value is a numeric type.
// Currently implemented are integer and float types.
bool isNumber(Value *value);

// Check that the value is an integer.
bool isInteger(Value *value);

// Check that the value is a double (float).
bool isDouble(Value *value);

// Check that the value is a boolean.
bool isBoolean(Value *value);

// Check that the value is a string.
bool isString(Value *value);

// Check that the value is a symbol.
bool isSymbol(Value *value);

// Check that the value is null.
bool isNull(Value *value);

// Check that the type of the valye is the given type.
// This is a generic function for less common type checks. For common types,
// like cons, use the associated function like isCons().
bool isType(Value *value, valueType type);

// Gets a Value's truth value: either true or false.
// All values except the Boolean type #f have the value true.
bool isTrue(Value *value);

#endif
