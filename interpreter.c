#include "interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

void interpret(Value *tree) {
    Value *current = tree;
    while(current->type != NULL_TYPE) {
        eval(current, )
        current = cdr(current);
    }
}

Value *eval(Value *expr, Frame *frame) {
    assert(expr != NULL);

    // Primitive types
    if (expr->type == INT_TYPE) {
        // return expr->i;
    } else if (expr->type == DOUBLE_TYPE) {
        // return expr->d;
    } else if (expr->type == STR_TYPE) {
        // return expr->s;
    } else if (expr->type == BOOL_TYPE) {
        // return expr->i;
    } else if (expr->type == NULL_TYPE) {

    } else if (expr->type == SYMBOL_TYPE) {
        lookUpSymbol(expr, frame);
    }
    // This means that the expression is a complete S-expression
    else if (expr->type == CONS_TYPE) {

        Value *first = car(expr);
        Value *args = cdr(expr);

        //TODO: sanity and error checking on first...
        // It should be a symbol type
        // None of these pointers should be null
        // cdr can be null_type (NOT NULL, which should be handled from above)

        // Special cases
        if (!strcmp(first->s, "if")) {
            return evalIf(args, frame);
        } else if (strcmp(first->s, "let")) {
            return evalLet(args, frame);
        } else {
            // ERROR
        }

        // Future assignments: do an actual function
    }
}