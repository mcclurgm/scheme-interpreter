#include "value.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "linkedlist.h"
#include "talloc.h"

// Allocates a Value struct and initializes its marked attribute to false.
Value *makeValue() {
    Value *newValue = talloc(sizeof(Value));
    newValue->marked = false;
    return newValue;
}

// Create a new NULL_TYPE value node.
Value *makeNull() {
    Value *null = makeValue();
    (*null).type = NULL_TYPE;
    return null;
}

// Create a new VOID_TYPE value node.
Value *makeVoid() {
    Value *result = makeValue();
    (*result).type = VOID_TYPE;
    return result;
}

// Create a new BOOL_TYPE Value.
Value *makeBool(bool val) {
    Value *result = makeValue();
    (*result).type = BOOL_TYPE;
    (*result).i = val;
    return result;
}

Value *makeInt(int val) {
    Value *result = makeValue();
    (*result).type = INT_TYPE;
    (*result).i = val;
    return result;
}

Value *makeDouble(double val) {
    Value *result = makeValue();
    (*result).type = DOUBLE_TYPE;
    (*result).d = val;
    return result;
}

Value *makeSymbol(char *val) {
    Value *result = makeValue();
    (*result).type = SYMBOL_TYPE;
    (*result).s = val;
    return result;
}

bool isList(Value *list) {
    Value *current = list;
    if (current->type != CONS_TYPE && current->type != NULL_TYPE) {
        return false;
    }

    while(current->type != NULL_TYPE) {
        if (cdr(current)->type != NULL_TYPE && cdr(current)->type != CONS_TYPE) {
            return false;
        }
        current = cdr(current);
    }

    return true;
}

bool isNumber(Value *value) {
    assert(value != NULL);
    return (value->type == INT_TYPE || value->type == DOUBLE_TYPE);
}