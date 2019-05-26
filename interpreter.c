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
        Value *result = eval(current, global);
        current = cdr(current);

        printValue(result);
        printf("\n");
    }
}

Value *lookupBindingInFrame(Value *symbol, Frame *frame) {
    Value *currentBinding = frame->bindings;
    while (currentBinding->type != NULL_TYPE) {
        // A binding is a linked list, where car points to a cons cell.
        // Car of that cell is the name of the binding, and cdr is its value.
        Value *bindingPair = car(currentBinding);
        if (!strcmp(car(bindingPair)->s, symbol->s)) {
            Value *bindingValue = cdr(bindingPair);
            return bindingValue;
        }
        currentBinding = cdr(currentBinding);
    }

    return NULL;
}

Value *lookUpSymbol(Value *symbol, Frame *activeFrame) {
    assert(symbol->type == SYMBOL_TYPE);

    // Check bindings in current frame
    Frame *currentFrame = activeFrame;
    while (currentFrame != NULL) {
        Value *search = lookupBindingInFrame(symbol, currentFrame);
        if (search != NULL) {
            Value *searchVal = car(search);
            return searchVal;
        } else {
            currentFrame = currentFrame->parent;
        }
    }

    // Error, not found
    printf("Symbol not found: %s\n", symbol->s);
    texit(1);
    return NULL;
}

Frame *makeBinding(Value *bindingPair, Frame *activeFrame) {
    Value *name = car(bindingPair);
    Value *expr = cdr(bindingPair);

    // Check that it's a pair
    if (expr->type == NULL_TYPE || cdr(expr)->type != NULL_TYPE) {
        printf("Binding in let statement is not a pair.\n");
        printf("At binding: ");
        printValue(bindingPair);
        printf("\n");
        texit(1);
    }

    if (name->type != SYMBOL_TYPE) {
        printf("Binding must assign a value to symbol; wrong token type found.\n");
        printf("At binding: ");
        printValue(bindingPair);
        printf("\n");
        texit(1);
    }

    // Check that this symbol isn't already bound in the current frame
    if (lookupBindingInFrame(name, activeFrame) != NULL) {
        printf("Duplicate binding in one let statement.\n");
        printf("At binding: ");
        printValue(bindingPair);
        printf(";\n");
        printf("For symbol: %s\n", name->s);
        texit(1);
    }

    Value *exprResult = eval(expr, activeFrame->parent);
    Value *newBinding = makeNull();
    newBinding = cons(exprResult, newBinding);
    newBinding = cons(name, newBinding);
    
    activeFrame->bindings = cons(newBinding, activeFrame->bindings);
    return activeFrame;
}

Value *evalDisplay(Value *argTree, Frame *activeFrame) {
    if (argTree->type != CONS_TYPE) {
        printf("display statement has no body: expected 1 argument, given none.\n");
        texit(1);
    }
    
    if (cdr(argTree)->type != NULL_TYPE) {
        printf("display statement has too many arguments: expected 1, given %i\n", length(argTree));
        printf("Expression: (display ");
        printTree(argTree);
        printf(")\n");
        texit(1);
    }

    // Evaluate the argument to display it
    Value *evalResult = eval(argTree, activeFrame);
    printValue(evalResult);

    Value *result = makeValue();
    result->type = VOID_TYPE;
    return result;
}

Value *evalWhen(Value *argsTree, Frame *activeFrame) {
    // Initialize expressions; sanity checks
    Value *condExpr = argsTree;
    if (condExpr->type != CONS_TYPE) {
        printf("When statement has no condition: expected at least 2 arguments, given none.\n");
        printf("Expression: (when ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    Value *thenExpr = cdr(condExpr);
    if (thenExpr->type != CONS_TYPE) {
        printf("when statement has no body: expected at least 2 arguments, given 1.\n");
        printf("Expression: (when ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *cond = eval(condExpr, activeFrame);
    if (cond->type != BOOL_TYPE) {
        printf("Error: condition of when statement must evaluate to boolean.\n");
        printf("Expression: (when ");
        printTree(argsTree);
        printf(")\n");
        printf("Condition expression: ");
        printValue(car(condExpr));
        printf("\n");
        texit(1);
    }

    if (cond->i) {
        // Condition is true
        // Evaluate the body expressions
        Value *result = makeNull();
        Value *currentExpr = thenExpr;
        while(currentExpr->type != NULL_TYPE) {
            result = eval(currentExpr, activeFrame);
            currentExpr = cdr(currentExpr);
        }
        return result;
    } else {
        Value *result = makeValue();
        result->type = VOID_TYPE;
        return result;
    }
}

Value *evalUnless(Value *argsTree, Frame *activeFrame) {
    // Initialize expressions; sanity checks
    Value *condExpr = argsTree;
    if (condExpr->type != CONS_TYPE) {
        printf("Unless statement has no condition: expected at least 2 arguments, given none.\n");
        printf("Expression: (unless ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    Value *thenExpr = cdr(condExpr);
    if (thenExpr->type != CONS_TYPE) {
        printf("Unless statement has no body: expected at least 2 arguments, given 1.\n");
        printf("Expression: (unless ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *cond = eval(condExpr, activeFrame);
    if (cond->type != BOOL_TYPE) {
        printf("Error: condition of unless statement must evaluate to boolean.\n");
        printf("Expression: (unless ");
        printTree(argsTree);
        printf(")\n");
        printf("Condition expression: ");
        printValue(car(condExpr));
        printf("\n");
        texit(1);
    }

    if (!cond->i) {
        // Condition is true
        // Evaluate the body expressions
        Value *result = makeNull();
        Value *currentExpr = thenExpr;
        while(currentExpr->type != NULL_TYPE) {
            result = eval(currentExpr, activeFrame);
            currentExpr = cdr(currentExpr);
        }
        return result;
    } else {
        Value *result = makeValue();
        result->type = VOID_TYPE;
        return result;
    }
}

Value *evalIf(Value *argsTree, Frame *activeFrame) {
    // Initialize expressions; sanity checks
    Value *condExpr = argsTree;
    if (condExpr->type != CONS_TYPE) {
        printf("If statement has no body: expected 3 arguments, given none.\n");
        printf("Expression: (if ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    Value *thenExpr = cdr(condExpr);
    if (thenExpr->type != CONS_TYPE) {
        printf("If statement has no 'then' expression: expected 3 arguments, given 1.\n");
        printf("Expression: (if ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    Value *elseExpr = cdr(thenExpr);
    if (elseExpr->type != CONS_TYPE) {
        printf("If statement has no 'else' expression: expected 3 arguments, given 2.\n");
        printf("Expression: (if ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *cond = eval(condExpr, activeFrame);
    if (cond->type != BOOL_TYPE) {
        printf("Condition of if statement must evaluate to boolean.\n");
        printf("Expression: (if ");
        printTree(argsTree);
        printf(")\n");
        printf("Condition expression: ");
        printValue(car(condExpr));
        printf("\n");
        texit(1);
    }

    Value *result = makeNull();
    if (cond->i) {
        // Condition is true
        result = eval(thenExpr, activeFrame);
    } else {
        result = eval(elseExpr, activeFrame);
    }
    return result;
}

Value *evalLet(Value *argsTree, Frame *activeFrame) {
    assert(argsTree != NULL);
    assert(activeFrame != NULL);
    if (argsTree->type != CONS_TYPE) {
        printf("let statement has no body; expected one.\n");
        printf("At expression: (let)\n");
        texit(1);
    }
    if (cdr(argsTree)->type == NULL_TYPE) {
        printf("let statement has no body; expected one.\n");
        printf("At expression: (let ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    if (car(argsTree)->type != CONS_TYPE) {
        printf("Bindings in let statement is not a list.\n");
        printf("At expression: (let ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Frame *letFrame = talloc(sizeof(Frame));
    letFrame->parent = activeFrame;
    letFrame->bindings = makeNull();

    // Make bindings
    Value *currentBindingPair = car(argsTree);
    while (currentBindingPair->type != NULL_TYPE) {
        if (car(currentBindingPair)->type != CONS_TYPE) {
            printf("Binding in let statement is not a pair.\n");
            printf("At expression: (let ");
            printTree(argsTree);
            printf(")\n");
            printf("At token: ");
            printValue(currentBindingPair);
            printf("\n");
            texit(1);
        }
        letFrame = makeBinding(car(currentBindingPair), letFrame);
        currentBindingPair = cdr(currentBindingPair);
    }
    
    // Evaluate the body expressions
    Value *result = makeNull();
    Value *currentExpr = cdr(argsTree);
    while(currentExpr->type != NULL_TYPE) {
        result = eval(currentExpr, letFrame);
        currentExpr = cdr(currentExpr);
    }
    
    return result;
}

Value *evalQuote(Value *argsTree, Frame *activeFrame) {
    assert(argsTree != NULL);
    if (argsTree->type != CONS_TYPE) {
        printf("quote statement has no body: expected 1 argument, given none.\n");
        texit(1);
    }
    
    if (cdr(argsTree)->type != NULL_TYPE) {
        printf("quote statement has too many arguments: expected 1, given %i\n", length(argsTree));
        printf("Expression: (quote ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    return car(argsTree);
}

Value *evalDefine(Value *argsTree, Frame *activeFrame) {
    assert(argsTree != NULL);
    if (argsTree->type != CONS_TYPE) {
        printf("Define statement has no body: expected 2 arguments, given none.\n");
        texit(1);
    }

    if (cdr(argsTree)->type == NULL_TYPE) {
        printf("Define statement has no expression to bind to.\n");
        printf("Found one argument; expected two.\n");
        printf("At expression: (define ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *symbol = car(argsTree);
    Value *expr = cdr(argsTree);

    if (symbol->type != SYMBOL_TYPE) {
        printf("Define must bind a value to symbol; wrong token type found for symbol.\n");
        printf("At expression: (define ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    if (cdr(expr)->type != NULL_TYPE) {
        printf("Define statement has too many arguments: expected 2, given %i\n", length(argsTree));
        printf("Expression: (define ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *exprResult = eval(expr, activeFrame);

    // Get the global frame
    Frame *globalFrame = activeFrame;
    while (globalFrame->parent != NULL) {
        globalFrame = globalFrame->parent;
    }

    Value *currentBindingValue = lookupBindingInFrame(symbol, globalFrame);
    if (currentBindingValue == NULL) {
        // Not already bound: make a new one
        Value *newBinding = makeNull();
        newBinding = cons(exprResult, newBinding);
        newBinding = cons(symbol, newBinding);
        
        globalFrame->bindings = cons(newBinding, globalFrame->bindings);
    } else {
        // Binding already exists
        // Set the value this binding points to to the new result
        currentBindingValue->c.car = exprResult;
    }

    Value *result = makeValue();
    result->type = VOID_TYPE;
    return result;
}

Value *eval(Value *tree, Frame *frame) {
    assert(tree != NULL);

    Value *expr = car(tree);

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
        assert(first != NULL);
        assert(args != NULL);
        if (first->type != SYMBOL_TYPE) {
            printf("First element of S-expression is not a symbol.\n");
            printf("At expression: ");
            printValue(first);
            printf("\n");
            texit(1);
        }

        // Special cases
        if (!strcmp(first->s, "if")) {
            return evalIf(args, frame);
        } else if (!strcmp(first->s, "let")) {
            return evalLet(args, frame);
        } else if (!strcmp(first->s, "display")) {
            return evalDisplay(args, frame);
        } else if (!strcmp(first->s, "when")) {
            return evalWhen(args, frame);
        } else if (!strcmp(first->s, "unless")) {
            return evalUnless(args, frame);
        } else if (!strcmp(first->s, "quote")) {
            return evalQuote(args, frame);
        } else if (!strcmp(first->s, "define")) {
            return evalDefine(args, frame);
        } else {
            // ERROR
            printf("Unrecognized symbol; expected function or special form\n");
            printf("At token: %s\n", first->s);
            texit(1);
        }

        // Future assignments: do an actual function
    }
    // The expression is of a type we don't know how to evaluate.
    // This is an error.
    printf("Error: you entered an unkown type of expression or token.\n");
    printf("Your friendly neighborhood interpreter developers don't know how to evaluate it.\n");
    printf("At expression: ");
    printValue(expr);
    printf("\n");
    texit(2);
    return NULL;
}