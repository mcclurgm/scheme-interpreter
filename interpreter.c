#include "interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

void interpret(Value *tree) {

}

Value *eval(Value *expr, Frame *frame) {
    if (expr->type == INT_TYPE) {

    } else if (....) {

    } else if (expr->type == SYMBOL_TYPE) {
        lookUpSymbol(expr, frame);
    } else if (expr->type == CONS_TYPE) {
        // This means that the expression is a complete S-expression

        Value *first = car(expr);
        Value *args = cdr(expr);

        //TODO: sanity and error checking on first...

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