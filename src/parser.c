#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "linkedlist.h"
#include "tokenizer.h"

bool tokenInExpression(Value *currentToken, valueType closeType) {
    if (currentToken->type == NULL_TYPE) {
        // We are trying to add the end of a file to an expression.
        // This corresponds to too many open parentheses: we never reached
        // a close paren to close the expression.
        // Throw a syntax error.
        printf("Syntax error: too many open parentheses.\n");
        texit(1);
    } else if (car(currentToken)->type == closeType) {
        return false;
    } else if (car(currentToken)->type == CLOSE_TYPE
            || car(currentToken)->type == CLOSE_BRACKET_TYPE) {

        // This means the first open token we hit is the wrong type.
        printf("Syntax error: bracket type mismatch.\n");
        texit(1);
    }
    return true;
}

Value *parseExpression(Value **currentToken) {
    valueType currentType = car(*currentToken)->type;

    if (currentType == OPEN_BRACKET_TYPE || currentType == OPEN_TYPE) {
        // The expression is cons-type, so iterate through all its sub-expressions
        // and cons them onto a subtree.
        valueType closeType;
        if (currentType == OPEN_TYPE) {
            closeType = CLOSE_TYPE;
        } else if (currentType == OPEN_BRACKET_TYPE) {
            closeType = CLOSE_BRACKET_TYPE;
        }

        // Iterate to the first token in the expression, after the open bracket
        *currentToken = cdr(*currentToken);

        Value *subtree = makeNull();
        while (tokenInExpression(*currentToken, closeType)) {
            Value *expr = parseExpression(currentToken);
            subtree = cons(expr, subtree);
            *currentToken = cdr(*currentToken);
        }

        return reverse(subtree);
    } else if (currentType == QUOTE_TYPE) {
        // Special case: quote syntax
        // Iterate the current token to whatever it's quoting
        *currentToken = cdr(*currentToken);

        // Parse that expression
        Value *expr = parseExpression(currentToken);

        // Construct a (quote expr) expression
        Value *quoteToken = makeSymbol("quote");

        Value *quoteExpr = cons(expr, makeNull());
        quoteExpr = cons(quoteToken, quoteExpr);

        return quoteExpr;
    } else {
        // This expression is just a single token
        Value *expr = car(*currentToken);
        return expr;
    }
}

// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {

    // Initialize an empty stack.
    Value *stack = makeNull();

    // While there are more tokens:
    Value *currentToken = tokens;
    assert(currentToken != NULL && "Error (parse): null pointer");
    while (currentToken->type != NULL_TYPE) {
        if (car(currentToken)->type == CLOSE_TYPE || car(currentToken)->type == CLOSE_BRACKET_TYPE) {
            // There are too many close parentheses.
            // All the close types should be handled by parseExpression,
            // since they're part of an expression (the end). So if we see a
            // close type, that means it's outside of an expression and is a
            // syntax error.
            printf("Syntax error: too many close parentheses\n");
            texit(1);
        }
        stack = cons(parseExpression(&currentToken), stack);
        currentToken = cdr(currentToken);
    }

    return reverse(stack);
}

void printValue(Value *val) {
    if (val->type == CONS_TYPE) {
        // This is a subtree expression.
        printf("(");
        printTree(val);
        printf(")");
    }
    if (isInteger(val)) {
        printf("%i", val->i);
    }
    else if (isDouble(val)) {
        printf("%f", val->d);
    }
    else if (val->type == STR_TYPE) {
        printf("\"%s\"", val->s);
    }
    else if (isSymbol(val)) {
        printf("%s", val->s);
    }
    else if (val->type == OPEN_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_TYPE\n");
    }
    else if (val->type == CLOSE_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_TYPE\n");
    }
    else if (isBoolean(val)) {
        if (val->i > 0) {
            printf("#t");
        } else {
            printf("#f");
        }
    }
    else if (val->type == QUOTE_TYPE) {
        printf("%s", val->s);
    }
    else if (val->type == OPEN_BRACKET_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_BRACKET_TYPE\n");
    }
    else if (val->type == CLOSE_BRACKET_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_BRACKET_TYPE\n");
    }
    else if (val->type == DOT_TYPE) {
        printf("%s", val->s);
    }
    else if (val->type == NULL_TYPE) {
        printf("()");
    }
    else if (val->type == PRIMITIVE_TYPE) {
        printf("#<primitive>");
    }
    else if (val->type == CLOSURE_TYPE) {
        printf("#<procedure>");

        // Debugging:
        // printf("Param names: ");
        // printValue(val->cl.paramNames);
        // printf("\n");
        // printf("Function code: ");
        // printTree(val->cl.functionCode);
        // printf("\n");
        // printf("Just trust the frame...\n");
    }
}

// Prints the tree to the screen in a readable fashion. It should look just like
// Racket code; use parentheses to indicate subtrees.
void printTree(Value *tree) {
    assert(tree != NULL);

    Value *current = tree;
    while(current->type != NULL_TYPE) {
        printValue(car(current));

        // If the next element is null (the current subtree is done),
        // don't print a space.
        // Otherwise, the next element requires a space.
        if (cdr(current)->type != NULL_TYPE) {
            printf(" ");
        }

        if (cdr(current)->type != NULL_TYPE && cdr(current)->type != CONS_TYPE) {
            printf(". ");
            printValue(cdr(current));
            break;
        }

        current = cdr(current);
    }
}

void printTreeTest(Value *tree, int indent) {
    assert(tree != NULL);

    Value *current = tree;
    while(current->type != NULL_TYPE) {
        if (car(current)->type != CONS_TYPE) {
            for(int i = 0; i < indent; i++) {
                printf(" ");
            }
        }

        if (car(current)->type == CONS_TYPE) {
            // This is a subtree expression.
            // printf("(\n");
            printf("CONS:\n");
            printTreeTest(car(current), indent + 4);
            // printf(")\n");
        }
        if (isInteger(car(current))) {
            printf("%i\n", car(current)->i);
        }
        else if (isDouble(car(current))) {
            printf("%f\n", car(current)->d);
        }
        else if (car(current)->type == STR_TYPE) {
            printf("\"%s\"\n", car(current)->s);
        }
        else if (isSymbol(car(current))) {
            printf("%s\n", car(current)->s);
        }
        else if (car(current)->type == OPEN_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_TYPE\n");
        }
        else if (car(current)->type == CLOSE_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_TYPE\n");
        }
        else if (isBoolean(car(current))) {
            if (car(current)->i > 0) {
                printf("#t\n");
            } else {
                printf("#f\n");
            }
        }
        else if (car(current)->type == QUOTE_TYPE) {
            printf("%s\n", car(current)->s);
        }
        else if (car(current)->type == OPEN_BRACKET_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_BRACKET_TYPE\n");
        }
        else if (car(current)->type == CLOSE_BRACKET_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_BRACKET_TYPE\n");
        }
        else if (car(current)->type == DOT_TYPE) {
            printf("%s\n", car(current)->s);
        }
        else if (car(current)->type == NULL_TYPE) {
            printf("() NULL TYPE\n");
        }

        printf(" ");
        current = cdr(current);
    }
}