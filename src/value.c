#include "value.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "linkedlist.h"
#include "talloc.h"

// Allocates a Value struct, sets its type, and initializes it as unmarked.
Value *makeValue(valueType type) {
    Value *newValue = talloc(sizeof(Value));
    newValue->marked = false;
    newValue->type = type;
    return newValue;
}

// Create a new NULL_TYPE value node.
Value *makeNull() {
    Value *null = makeValue(NULL_TYPE);
    return null;
}

// Create a new VOID_TYPE value node.
Value *makeVoid() {
    Value *result = makeValue(VOID_TYPE);
    return result;
}

// Create a new BOOL_TYPE Value.
Value *makeBool(bool val) {
    Value *result = makeValue(BOOL_TYPE);
    (*result).i = val;
    return result;
}

Value *makeInt(int val) {
    Value *result = makeValue(INT_TYPE);
    (*result).i = val;
    return result;
}

Value *makeDouble(double val) {
    Value *result = makeValue(DOUBLE_TYPE);
    (*result).d = val;
    return result;
}

Value *makeString(char *val) {
    Value *result = makeValue(STR_TYPE);
    (*result).s = val;
    return result;
}

Value *makeSymbol(char *val) {
    Value *result = makeValue(SYMBOL_TYPE);
    (*result).s = val;
    return result;
}

// Check that the value is the head of a proper list structure.
//TODO Note, this does not check for circular lists.
bool isProperList(Value *value) {
    Value *current = value;
    // Check that the current value is either a cons cell or null
    if (current->type != CONS_TYPE && current->type != NULL_TYPE) {
        return false;
    }

    // Check that each subsequent value also starts a valid list
    while(current->type != NULL_TYPE) {
        if (cdr(current)->type != NULL_TYPE && cdr(current)->type != CONS_TYPE) {
            return false;
        }
        current = cdr(current);
    }

    return true;
}

// Check that the value is a numeric type.
// Currently implemented are integer and float types.
bool isNumber(Value *value) {
    assert(value != NULL);
    return (isInteger(value) || isDouble(value));
}

// Check that the value is an integer.
bool isInteger(Value *value) {
    assert(value != NULL);
    return value->type == INT_TYPE;
}

// Check that the value is a double (float).
bool isDouble(Value *value) {
    assert(value != NULL);
    return value->type == DOUBLE_TYPE;
}

// Check that the value is a boolean.
bool isBoolean(Value *value) {
    assert(value != NULL);
    return value->type == BOOL_TYPE;
}

// Check that the value is a symbol.
bool isSymbol(Value *value) {
    assert(value != NULL);
    return value->type == SYMBOL_TYPE;
}

// Gets a Value's truth value: either true or false.
// All values except the Boolean type #f have the value true.
bool isTrue(Value *value) {
    assert(value != NULL);
    if (value->type != BOOL_TYPE) {
        return true;
    } else {
        return value->i;
    }
}
