#include "interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"

Value *primitiveAdd(Value *args) {
    double sum = 0;
    bool isInt = true;
    Value *current = args;
    //Pseudocode like stuff
    while(current->type != NULL_TYPE) {
        if(car(current)->type != INT_TYPE && car(current)->type != DOUBLE_TYPE) {
            printf("Expected number in +\n");
            printf("Given: ");
            printTree(current);
            printf("\n");
            texit(1);
        } else if (car(current)->type == INT_TYPE) {
            sum += car(current)->i;
        } else {
            sum += car(current)->d;
            isInt = false;
        }
        current = cdr(current);
    }

    Value *result = makeValue();
    if(isInt){
        result->type = INT_TYPE;
        result->i = sum;
    } else {
        result->type = DOUBLE_TYPE;
        result->d = sum;
    }
    
    return result;
}

Value *primitiveNull(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("null? statement has no body: expected 1 argument, given none.\n");
        texit(1);
    }
    
    if (cdr(args)->type != NULL_TYPE) {
        printf("null? statement has too many arguments: expected 1, given %i\n", length(args));
        printf("Expression: (null? ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *result = makeValue();
    result->type = BOOL_TYPE;

    if (car(args)->type == NULL_TYPE) {
        result->i = true;
    } else {
        result->i = false;
    }

    return result;
}

Value *primitiveCar(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("car statement has no body: expected 1 argument, given none.\n");
        texit(1);
    }
    
    if (cdr(args)->type != NULL_TYPE) {
        printf("car statement has too many arguments: expected 1, given %i\n", length(args));
        printf("Expression: (car ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    if (car(args)->type != CONS_TYPE) {
        printf("car statement needs to act on a cons cell\n");
        printf("Expression: (car ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *result = makeValue();
    result = car(car(args));

    return result;
}

Value *primitiveCdr(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("cdr statement has no body: expected 1 argument, given none.\n");
        texit(1);
    }
    
    if (cdr(args)->type != NULL_TYPE) {
        printf("cdr statement has too many arguments: expected 1, given %i\n", length(args));
        printf("Expression: (cdr ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    if (car(args)->type != CONS_TYPE) {
        printf("cdr statement needs to act on a cons cell\n");
        printf("Expression: (cdr ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *result = makeValue();
    result = cdr(car(args));

    return result;
}

void bindPrimitive(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
	Value *symbol = makeValue();
	symbol->type = SYMBOL_TYPE;
	symbol->s = name;

    Value *value = talloc(sizeof(Value));
    value->type = PRIMITIVE_TYPE;
    value->pf = function;
    
	Value *binding = makeNull();
	binding = cons(value, binding);
	binding = cons(symbol, binding);
	frame->bindings = cons(binding, frame->bindings);
}

void interpret(Value *tree) {
    Frame *global = talloc(sizeof(Frame));
    global->parent = NULL;
    global->bindings = makeNull();

	bindPrimitive("+", primitiveAdd, global);
    bindPrimitive("null?", primitiveNull, global);
    bindPrimitive("car", primitiveCar, global);
    bindPrimitive("cdr", primitiveCdr, global);

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

bool checkValidParameters(Value *paramsList) {
    if (paramsList->type == NULL_TYPE) {
        return true;
    }
    if (paramsList->type == SYMBOL_TYPE) {
        return true;
    }
    
    Value *currentParam = paramsList;
    if (car(currentParam)->type != SYMBOL_TYPE) {
        return false;
    }
    // Check that parameter name can't show up twice
    while (currentParam->type != NULL_TYPE) {
        Value *compareParam = cdr(currentParam);
        while (compareParam->type != NULL_TYPE) {
            if (car(compareParam)->type != SYMBOL_TYPE) {
                return false;
            }
            if (!strcmp(car(compareParam)->s, car(currentParam)->s)) {
                // A name appears twice in the list
                return false;
            }
            compareParam = cdr(compareParam);
        }
        currentParam = compareParam;
    }
    return true;
}

// Yes, we know how this name sounds...
Frame *makeApplyBindings(Value *functionParams, Value *args, Frame *functionFrame) {
    if (functionParams->type == SYMBOL_TYPE) {
            Value *newBinding = makeNull();
            newBinding = cons(args, newBinding);
            newBinding = cons(functionParams, newBinding);
            functionFrame->bindings = cons(newBinding, functionFrame->bindings);
    } else {
        assert(functionParams->type == CONS_TYPE || functionParams->type == NULL_TYPE);

        if (length(functionParams) != length(args)) {
            printf("Arity mismatch in function application.\n");
            printf("Function expected %i arguments, ", length(functionParams));
            printf("given %i.\n", length(args));
            texit(1);
        }
        
        Value *currentParam = functionParams;
        Value *currentArg = args;

        // Iterate through both at once.
        // Bind the current param to the current arg.
        // Note: we can now assume that functionParams and args have the same length.
        // Also, functionParams can't include duplicate symbols to bind. That was
        // handled in evalLambda.
        while (currentParam->type != NULL_TYPE) {
            // Make the binding
            Value *newBinding = makeNull();
            newBinding = cons(car(currentArg), newBinding);
            newBinding = cons(car(currentParam), newBinding);
            functionFrame->bindings = cons(newBinding, functionFrame->bindings);

            currentParam = cdr(currentParam);
            currentArg = cdr(currentArg);
        }
    }
    return functionFrame;
}

Value *apply(Value *function, Value *argsTree) {
    //Sanity checks for closure

    if (function->type != CLOSURE_TYPE) {
        printf("Application not a procedure.\n");
        printf("Given: ");
        printValue(function);
        printf("\n");
        printf("This is not a procedure\n");
        texit(1);
    }

    // Construct a new frame whose parent is the environment stored in the closure (function)
    Frame *evalFrame = talloc(sizeof(Frame));
    evalFrame->parent = function->cl.frame;
    evalFrame->bindings = makeNull();

    // Bind parameters to arguments in this frame
    evalFrame = makeApplyBindings(function->cl.paramNames, argsTree, evalFrame);

    // Evaluate the function body expressions
    Value *result = makeNull();
    Value *currentExpr = function->cl.functionCode;
    while(currentExpr->type != NULL_TYPE) {
        result = eval(currentExpr, evalFrame);
        currentExpr = cdr(currentExpr);
    }
    
    return result;
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

    if (car(argsTree)->type != CONS_TYPE && car(argsTree)->type != NULL_TYPE) {
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

Value *evalLambda(Value *argsTree, Frame *activeFrame) {
    assert(argsTree != NULL);
    assert(activeFrame != NULL);
    
    if (argsTree->type != CONS_TYPE) {
        printf("Lambda statement has no or invalid parameters.\n");
        printf("Expected either a list or ().");
        printf("At expression: (lambda ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    if (cdr(argsTree)->type == NULL_TYPE) {
        printf("Lambda statement has no body; expected one.\n");
        printf("At expression: (lambda ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *params = car(argsTree);
    Value *body = cdr(argsTree);

    // Check parameters list: can be cons or null
    if (params->type != CONS_TYPE && params->type != NULL_TYPE 
            && params->type != SYMBOL_TYPE) {
        printf("Parameters in lambda statement are not a list.\n");
        printf("At expression: (lambda ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    if (!checkValidParameters(params)) {
        printf("Ill-formed parameter list.\n");
        printf("Must be a list of unique symbols.\n");
        printf("At expression: (lambda ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *closure = makeValue();
    closure->type = CLOSURE_TYPE;
    closure->cl.frame = activeFrame;
    closure->cl.paramNames = params;
    closure->cl.functionCode = body;
    return closure;
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

    Value *symbol;
    Value *exprResult;
    if (car(argsTree)->type == CONS_TYPE) {
        symbol = car(car(argsTree));
        if (symbol->type != SYMBOL_TYPE) {
            printf("Function name must be a symbol; wrong token type found.\n");
            printf("At expression: (define ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }

        Value *params = cdr(car(argsTree));
        Value *body = cdr(argsTree);

        if (body->type != CONS_TYPE) {
            printf("No function body found.\n");
            printf("At expression: (define ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }

        Value *lambdaExpr = cons(params, body);
        exprResult = evalLambda(lambdaExpr, activeFrame);
    } else if (car(argsTree)->type == SYMBOL_TYPE) {
        symbol = car(argsTree);
        Value *expr = cdr(argsTree);

        if (cdr(expr)->type != NULL_TYPE) {
            printf("Define statement has too many arguments: expected 2, given %i\n", length(argsTree));
            printf("Expression: (define ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }

        exprResult = eval(expr, activeFrame);
    } else {
        printf("Define must bind a value to symbol; wrong token type found for symbol name.\n");
        printf("At expression: (define ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

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

/*
Evaluates each expression in body.
Returns a linked list of all the expression results.
*/
Value *evalEach(Value *body, Frame *activeFrame) {
    Value *result = makeNull();
    Value *currentExpr = body;
    while(currentExpr->type != NULL_TYPE) {
        result = cons(eval(currentExpr, activeFrame), result);
        currentExpr = cdr(currentExpr);
    }

    return reverse(result);
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
    } else if (expr->type == PRIMITIVE_TYPE) {
        return expr;
    } else if (expr->type == NULL_TYPE) {
        return expr;
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


        if (first->type == SYMBOL_TYPE) {
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
            } else if (!strcmp(first->s, "lambda")) {
                return evalLambda(args, frame);
            // Otherwise, proceed with standard evaluation
            } else {
                // If not a special form, evaluate the first, evaluate the args,
                // then apply the first to the args.
                Value *evaledOperator = eval(expr, frame);
                Value *evaledArgs = evalEach(args, frame);
                if (evaledOperator->type == PRIMITIVE_TYPE) {
                    return (*(evaledOperator->pf))(evaledArgs);
                } else {
                    return apply(evaledOperator,evaledArgs);
                }
            }
        } else {
            // If not a special form, evaluate the first, evaluate the args,
            // then apply the first to the args.
            Value *evaledOperator = eval(expr, frame);
            Value *evaledArgs = evalEach(args, frame);
            if (evaledOperator->type == PRIMITIVE_TYPE) {
                    return (*(evaledOperator->pf))(evaledArgs);
                } else {
                    return apply(evaledOperator,evaledArgs);
                }
        }
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