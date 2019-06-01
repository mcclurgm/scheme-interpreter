#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "linkedlist.h"
#include "tokenizer.h"

Value *addTokenToTree(Value *token, Value *tree, int *depth) {
    tree = cons(token, tree);

    // If adding a paren to the tree, this means we've gone one more level in depth.
    if (token->type == OPEN_TYPE || token->type == OPEN_BRACKET_TYPE) {
        *depth += 1;
    }

    return tree;
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
        if (car(current)->type == INT_TYPE) {
            printf("%i\n", car(current)->i);
        }
        else if (car(current)->type == DOUBLE_TYPE) {
            printf("%f\n", car(current)->d);
        }
        else if (car(current)->type == STR_TYPE) {
            printf("\"%s\"\n", car(current)->s);
        }
        else if (car(current)->type == SYMBOL_TYPE) {
            printf("%s\n", car(current)->s);
        }
        else if (car(current)->type == OPEN_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_TYPE\n");
        }
        else if (car(current)->type == CLOSE_TYPE) {
            printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_TYPE\n");
        }
        else if (car(current)->type == BOOL_TYPE) {
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

/*
Checks if the currentItem would end a subtree (ie, if it is an open of some sort).

This also checks for the too many close parentheses syntax error: if currentItem
is NULL_TYPE, that means we've gone all the way to the beginning of the file 
without hitting an open paren to match. In this case, it throws a syntax error
and exits the program.
*/
bool canBuildSubtreeFrom(Value *currentItem, valueType openType) {
    if (currentItem->type == NULL_TYPE) {
        // We are trying to add the beginning of a file to the subtree.
        // This corresponds to too many close parentheses: we never reached
        // an open paren to stop this from happening.
        // Throw a syntax error.
        printf("Syntax error: too many close parentheses\n");
        texit(1);
    } else if (car(currentItem)->type == openType) {
        return false;
    } else if (car(currentItem)->type == OPEN_TYPE 
            || car(currentItem)->type == OPEN_BRACKET_TYPE) {
        
        // This means the first open token we hit is the wrong type.
        printf("Syntax error: bracket type mismatch.\n");
        texit(1);
    }
    return true;
}

/*
Call this when you hit a close paren.
It pops tokens off the main stack until it hits an open paren,
building a new list of those elements.
It then adds the resulting list to the front of the stack, as car of a new Value.

In order to match the correct type of brackets, this needs the type of open to 
cheeck for. To do this, pass the type of the close token into the function.
*/
Value *makeNewSubtree(Value *tree, int *depth, valueType closeType) {
    Value *subtree = makeNull();

    valueType openType;
    if (closeType == CLOSE_TYPE) {
        openType = OPEN_TYPE;
    } else if (closeType == CLOSE_BRACKET_TYPE) {
        openType = OPEN_BRACKET_TYPE;
    } else {
        printf("Error in parser: makeNewSubtree did not receive a valid close type\n");
        texit(2); // Let's say 2 means an internal error
    }

    Value *currentToken = tree;
    // While the token is not an open:
    while (canBuildSubtreeFrom(currentToken, openType)) {
        subtree = cons(car(currentToken), subtree);
        currentToken = cdr(currentToken);
    }

    // Set up the resulting tree by making the next element after subtree
    // the token before the open paren.
    // This is the equivalent of popping all the items above this off the stack.
    Value *newCdr = cdr(currentToken);
    Value *newTree = cons(subtree, newCdr);

    *depth -= 1;
    return newTree;
}

// Takes a list of tokens from a Racket program, and returns a pointer to a
// parse tree representing that program.
Value *parse(Value *tokens) {
    // Initialize an empty stack.
    Value *stack = makeNull();

    // While there are more tokens:
    Value *currentToken = tokens;
    assert(currentToken != NULL && "Error (parse): null pointer");
    int depth = 0;
    while (currentToken->type != NULL_TYPE) {
        if (car(currentToken)->type == CLOSE_TYPE 
                || car(currentToken)->type == CLOSE_BRACKET_TYPE) {
            
            stack = makeNewSubtree(stack, &depth, car(currentToken)->type);
        } else {
            stack = addTokenToTree(car(currentToken), stack, &depth);
        }
        currentToken = cdr(currentToken);
    }

    // Check for not enough close parens case
    if (depth != 0) {
        printf("Syntax error: not enough close parentheses.\n");
        texit(1);
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
    if (val->type == INT_TYPE) {
        printf("%i", val->i);
    }
    else if (val->type == DOUBLE_TYPE) {
        printf("%f", val->d);
    }
    else if (val->type == STR_TYPE) {
        printf("\"%s\"", val->s);
    }
    else if (val->type == SYMBOL_TYPE) {
        printf("%s", val->s);
    }
    else if (val->type == OPEN_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO OPEN_TYPE\n");
    }
    else if (val->type == CLOSE_TYPE) {
        printf("THIS IS A MAJOR ERROR THERE SHOULD BE NO CLOSE_TYPE\n");
    }
    else if (val->type == BOOL_TYPE) {
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