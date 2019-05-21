#include "interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"

void interpret(Value *tree) {
    Frame *global = talloc(sizeof(Frame));
    global->parent = NULL;
    global->bindings = makeNull();

    printf("Original tree:  ");
    printTree(tree);
    printf("\n");
    
    Value *current = tree;
    while(current->type != NULL_TYPE) {
        Value *result = eval(car(current), global);
        current = cdr(current);

        printValue(result);
        printf("\n");
    }
}

Value *lookUpSymbol(Value *symbol, Frame *activeFrame) {
    assert(symbol->type == SYMBOL_TYPE);

    // Check bindings in current frame
    Frame *currentFrame = activeFrame;
    while (currentFrame != NULL) {
        Value *bindings = currentFrame->bindings;

        Value *currentBinding = bindings;
        while (currentBinding->type != NULL_TYPE) {
            // A binding is a linked list, where car points to a cons cell.
            // Car of that cell is the name of the binding, and cdr is its value.
            Value *bindingPair = car(currentBinding);
            if (!strcmp(car(bindingPair)->s, symbol->s)) {
                return cdr(bindingPair);
            }
            currentBinding = cdr(currentBinding);
        }
        
        currentFrame = currentFrame->parent;
    }

    // Error, not found
    printf("Symbol not found: %s\n", symbol->s);
    texit(1);
}

Value *evalIf(Value *tree, Frame *activeFrame) {
    assert(tree->type == CONS_TYPE);
    printf("Implement evalIf\n");
}

Value *evalLet(Value *tree, Frame *activeFrame) {
    assert(tree->type == CONS_TYPE);
    printf("Implement evalLet\n");
}

Value *eval(Value *expr, Frame *frame) {
    assert(expr != NULL);

    // Primitive types
    if (expr->type == INT_TYPE) {
        return expr;
    } else if (expr->type == DOUBLE_TYPE) {
        return expr;
    } else if (expr->type == STR_TYPE) {
        return expr;
    } else if (expr->type == BOOL_TYPE) {
        return expr;
    } else if (expr->type == NULL_TYPE) {
        //TODO We'll figure this out later.
        printf("NULL figure this out\n");
        texit(2);
    } else if (expr->type == SYMBOL_TYPE) {
        return lookUpSymbol(expr, frame);
    }
    // This means that the expression is a complete S-expression
    else if (expr->type == CONS_TYPE) {

        Value *first = car(expr);
        Value *args = cdr(expr);

        //TODO: sanity and error checking on first...
        // It should be a symbol type
        // None of these pointers should be null
        // cdr can be null_type (NOT NULL, which should be handled from above)

        printf("First type:%i\n", first->type);
        // Special cases
        if (!strcmp(first->s, "if")) {
            return evalIf(args, frame);
        } else if (!strcmp(first->s, "let")) {
            return evalLet(args, frame);
        } else {
            // ERROR
            printf("Unrecognized symbol; expected function or special form\n");
            printf("At token: %s\n", first->s);
            texit(1);
        }

        // Future assignments: do an actual function
    }
}