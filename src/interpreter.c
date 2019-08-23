#include "interpreter.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "tokenizer.h"

//==================
// Helper Functions
//==================

/* Compares the value of two number-type Values.
 *
 * Assumes that both arguments are numbers.
 * Check this before you call it.
 */
bool compareNumbers(Value *one, Value *two) {
    if (isInteger(one)) {
        if (isInteger(two)) {
            return one->i == two->i;
        } else {
            return one->i == two->d;
        }
    } else {
        if (isInteger(two)) {
            return one->d == two->i;
        } else {
            return one->d == two->d;
        }
    }
}

/* Stops execution with an error if given the wrong arity, for use in predicates.
 *
 * Checks that the args list has length numArgs. If it does not, stops execution
 * and prints a helpful error message to the console.
 *
 * This function enforces a single acceptable arity, useful for predicates
 * like not, <, car, if, etc. To enforce a range of acceptable arities, use
 * enforceArgumentArityRange.
 *
 * args: the cons cell containing the arguments passed to a predicate.
 *       Can be obtained using cdr(expression).
 * numArgs: the number of arguments to be enforced.
 * expressionType: the name of the predicate, for use in error reporting.
 */
void enforceArgumentArity(Value *args, int numArgs, char *expressionType) {
    int arity = length(args);
    if (arity == 0 && numArgs > 0) {
        printf("Arity mismatch.\n");
        printf("%s expression has no body: expected at least %i argument, "
               "given none.\n",
               expressionType, numArgs);
        printf("Expression: (%s", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (arity < numArgs) {
        printf("Arity mismatch.\n");
        printf("%s expression has too few arguments: expected %i, given %i\n",
               expressionType, numArgs, arity);
        printf("Expression: (%s ", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    } else if (arity > numArgs) {
        printf("Arity mismatch.\n");
        printf("%s expression has too many arguments: expected %i, given %i\n",
               expressionType, numArgs, arity);
        printf("Expression: (%s ", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    }
}

/* Stops execution with an error if given the wrong arity; supports ranges.
 *
 * Checks that the args list has length between minArgs and maxArgs. If it does
 * not, stops execution and prints a helpful error message to the console.
 *
 * This function enforces a range of acceptable arities. This range can be
 * bounded or not. To set an unbounded min and/or max number of arguments, set
 * the corresponding parameter to negative (typically -1). For example, = requires a minimum of two arguments. It can
 * accept an arbitrary number of arguments larger than two, so its maximum is
 * unbounded. To support this, use minArgs = 2 and maxArgs = -1. It also
 * supports a minimum of zero arguments, like * that can accept zero or more.
 *
 * args: the cons cell containing the arguments passed to a predicate.
 *       Can be obtained using cdr(expression).
 * minArgs: the minimum number of arguments to be enforced.
 *          A negative value sets the range to have no minimum (be unbounded
 *          at the lower end).
 * maxArgs: the maximum number of arguments to be enforced.
 *          A negative value sets the range to have no maximum (be unbounded
 *          at the upper end).
 * expressionType: the name of the predicate, for use in error reporting.
 */
void enforceArgumentArityRange(Value *args, int minArgs, int maxArgs,
                               char *expressionType) {
    int arity = length(args);
    if (arity == 0 && minArgs > 0) {
        printf("Arity mismatch.\n");
        printf("%s expression has no body: expected at least %i argument, "
               "given none.",
               expressionType, minArgs);
        printf("Expression: (%s ", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (minArgs >= 0 && arity < minArgs) {
        printf("Arity mismatch.\n");
        printf("%s expression has too few arguments: expected at least %i, given %i\n",
               expressionType, minArgs, arity);
        printf("Expression: (%s ", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    } else if (maxArgs >= 0 && arity > maxArgs) {
        printf("Arity mismatch.\n");
        printf("%s expression has too many arguments: expected at most %i, given %i\n",
               expressionType, maxArgs, arity);
        printf("Expression: (%s ", expressionType);
        printTree(args);
        printf(")\n");
        texit(1);
    }
}

/* Returns the global frame of the evaluation environment.
 *
 * Iterates through the parents of activeFrame until it finds the global,
 * which does not have a parent. This is denoted by a NULL pointer.
 */
Frame *getGlobalFrame(Frame *activeFrame) {
    Frame *globalFrame = activeFrame;
    while (globalFrame->parent != NULL) {
        globalFrame = globalFrame->parent;
    }
    return globalFrame;
}

/*
 * Evaluates each expression in body.
 * Returns a linked list of all the expression results.
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

//==================
// Primitives
//==================

Value *primitiveAdd(Value *args) {
    double sum = 0;
    bool isInt = true;

    Value *current = args;
    while(current->type != NULL_TYPE) {
        if(car(current)->type != INT_TYPE && car(current)->type != DOUBLE_TYPE) {
            printf("Expected number in +\n");
            printf("Given: ");
            printTree(current);
            printf("\n");
            texit(1);
        } else if (isInteger(car(current))) {
            sum += car(current)->i;
        } else {
            sum += car(current)->d;
            isInt = false;
        }
        current = cdr(current);
    }

    Value *result;
    if(isInt){
        result = makeInt(sum);
    } else {
        result = makeDouble(sum);
    }

    return result;
}

Value *primitiveSubtract(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("- expression has no arguments: expected at least 2, given none.\n");
        texit(1);
    }
    if (cdr(args)->type != CONS_TYPE) {
        printf("- expression has too few arguments: expected at least 2, given 1\n");
        printf("Expression: (- ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    double difference = 0;
    bool isInt = true;
    if(car(args)->type != INT_TYPE && car(args)->type != DOUBLE_TYPE) {
        printf("Expected number in +\n");
        printf("Given: ");
        printTree(args);
        printf("\n");
        texit(1);
    } else if (isInteger(car(args))) {
        difference = car(args)->i;
    } else {
        difference = car(args)->d;
        isInt = false;
    }

    Value *current = cdr(args);
    while(current->type != NULL_TYPE) {
        if(car(current)->type != INT_TYPE && car(current)->type != DOUBLE_TYPE) {
            printf("Expected number in +\n");
            printf("Given: ");
            printTree(current);
            printf("\n");
            texit(1);
        } else if (isInteger(car(current))) {
            difference -= car(current)->i;
        } else {
            difference -= car(current)->d;
            isInt = false;
        }
        current = cdr(current);
    }

    Value *result;
    if(isInt){
        result = makeInt(difference);
    } else {
        result = makeDouble(difference);
    }

    return result;
}

Value *primitiveMult(Value *args) {
    double product = 1;
    bool isInt = true;

    Value *current = args;
    while(current->type != NULL_TYPE) {
        if(car(current)->type != INT_TYPE && car(current)->type != DOUBLE_TYPE) {
            printf("Expected number in *\n");
            printf("Given: ");
            printTree(current);
            printf("\n");
            texit(1);
        } else if (isInteger(car(current))) {
            product *= car(current)->i;
        } else {
            product *= car(current)->d;
            isInt = false;
        }
        current = cdr(current);
    }

    // Package result in a Value
    Value *result;
    if(isInt){
        result = makeInt(product);
    } else {
        result = makeDouble(product);
    }

    return result;
}

Value *primitiveDivide(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("/ expression has no arguments: expected 2, given none.\n");
        texit(1);
    }
    if (cdr(args)->type != CONS_TYPE) {
        printf("/ expression has too few arguments: expected 2, given 1\n");
        printf("Expression: (/ ");
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (cdr(cdr(args))->type != NULL_TYPE) {
        printf("/ statement has too many arguments: expected 2, given %i\n", length(args));
        printf("Expression: (/ ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *result;
    Value *numerator = car(args);
    Value *denominator = car(cdr(args));
    if (isInteger(numerator)) {
        if (isInteger(denominator)) {
            if (numerator->i % denominator->i != 0) {
                double dividend = (1.0 * numerator->i) / denominator->i;
                result = makeDouble(dividend);
            } else {
                int dividend = numerator->i / denominator->i;
                result = makeInt(dividend);
            }
        } else if (isDouble(denominator)) {
            result = makeDouble(numerator->i / denominator->d);
        } else {
            printf("Expected number in /\n");
            printf("Given: ");
            printValue(denominator);
            printf("\n");
            printf("At expression: (/ ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else  if (isDouble(numerator)) {
        if (isInteger(denominator)) {
            result = makeDouble(numerator->d / denominator->i);
        } else if (isDouble(denominator)) {
            result = makeDouble(numerator->d / denominator->d);
        } else {
            printf("Expected number in /\n");
            printf("Given: ");
            printValue(denominator);
            printf("\n");
            printf("At expression: (/ ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else {
        printf("Expected number in /\n");
        printf("Given: ");
        printValue(numerator);
        printf("\n");
        printf("At expression: (/ ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    return result;
}

Value *primitiveIsNull(Value *args) {
    enforceArgumentArity(args, 1, "null?");

    if (car(args)->type == NULL_TYPE) {
        return makeBool(true);
    } else {
        return makeBool(false);
    }
}

Value *primitiveIsList(Value *args) {
    enforceArgumentArity(args, 1, "list?");

    Value *listVal = car(args);
    return makeBool(isProperList(listVal));
}

Value *primitiveCar(Value *args) {
    enforceArgumentArity(args, 1, "cons");
    if (car(args)->type != CONS_TYPE) {
        printf("car statement needs to act on a cons cell\n");
        printf("Expression: (car ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    return car(car(args));
}

Value *primitiveCdr(Value *args) {
    enforceArgumentArity(args, 1, "cdr");
    if (car(args)->type != CONS_TYPE) {
        printf("cdr statement needs to act on a cons cell\n");
        printf("Expression: (cdr ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    return cdr(car(args));
}

Value *primitiveCons(Value *args) {
    enforceArgumentArity(args, 2, "cons");

    Value *result = cons(car(args), car(cdr(args)));
    return result;
}

Value *primitiveList(Value *args) {
    Value *result = args;
    return result;
}

Value *primitiveAppend(Value *args) {
    if (args->type != CONS_TYPE) {
        return makeNull();
    }

    Value *orderedArgs = reverse(args);

    Value *currentList = car(orderedArgs);
    Value *currentArg = cdr(orderedArgs);
    while (currentArg->type != NULL_TYPE) {
        currentList = append(car(currentArg), currentList);
        currentArg = cdr(currentArg);
    }
    return currentList;
}

Value *primitiveReverse(Value *args) {
    enforceArgumentArity(args, 1, "reverse");
    if (car(args)->type != CONS_TYPE && car(args)->type != NULL_TYPE) {
        printf("reverse expression needs to act on a cons cell\n");
        printf("Expression: (reverse ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    return reverse(car(args));
}

Value *primitiveLength(Value *args) {
    enforceArgumentArity(args, 1, "length");

    // Null is an empty (zero-length) list
    if (car(args)->type == NULL_TYPE) {
        return makeInt(0);
    }

    if (car(args)->type != CONS_TYPE) {
        printf("length expression needs to act on a cons cell\n");
        printf("Expression: (length ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    int len = length(car(args));
    return makeInt(len);
}

Value *primitiveEqual(Value *args) {
    enforceArgumentArity(args, 2, "equal?");

    Value *first = car(args);
    Value *second = car(cdr(args));

    if (first->type != second->type) {
        return makeBool(false);
    }

    if (isInteger(first) || isBoolean(first)) {
        // Booleans are really C ints, so they can be treated identically
        if (first->i == second->i) {
            return makeBool(true);
        } else {
            return makeBool(false);
        }
    } else if (isDouble(first)) {
        if (first->d == second->d) {
            return makeBool(true);
        } else {
            return makeBool(false);
        }
    } else if (first->type == STR_TYPE || first->type == SYMBOL_TYPE) {
        if (!strcmp(first->s, second->s)) {
            return makeBool(true);
        } else {
            return makeBool(false);
        }
    } else {
        printf("Unknown type in equal?\n");
        printf("given: ");
        printValue(first);
        printf("\n");
        texit(1);
    }
    assert(false && "Internal error: should have returned or texited out of if/else block but did not");
}

Value *primitiveEq(Value *args) {
    enforceArgumentArity(args, 2, "eq?");

    Value *first = car(args);
    Value *second = car(cdr(args));

    if (first == second) {
        return makeBool(true);
    } else {
        return makeBool(false);
    }
}

Value *primitiveIsNumber(Value *args) {
    enforceArgumentArity(args, 1, "equal?");

    Value *val = car(args);
    return makeBool(isNumber(val));
}

Value *primitiveEqualNum(Value *args) {
    enforceArgumentArityRange(args, 1, -1, "=");

    if (args->type != CONS_TYPE) {
        printf("= statement has no arguments: expected at least one.\n");
        printf("At: (=)\n");
        texit(1);
    }

    Value *first = car(args);
    if(first->type != INT_TYPE && first->type != DOUBLE_TYPE) {
        printf("Expected number in =\n");
        printf("Given ");
        printValue(first);
        printf("\n");
        printf("At expression: (= ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *current = cdr(args);
    while (current->type != NULL_TYPE) {
        Value *currentValue = car(current);
        if(currentValue->type != INT_TYPE && currentValue->type != DOUBLE_TYPE) {
            printf("Expected number in =\n");
            printf("Given ");
            printValue(currentValue);
            printf("\n");
            printf("At expression: (= ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
        if (!compareNumbers(first, currentValue)) {
            return makeBool(false);
        }
        current = cdr(current);
    }

    // If we made it here, they must be equal
    return makeBool(true);
}

Value *primitiveLessThan(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("< expression has no arguments: expected 2, given none.\n");
        texit(1);
    }
    if (cdr(args)->type != CONS_TYPE) {
        printf("< expression has too few arguments: expected 2, given 1\n");
        printf("Expression: (< ");
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (cdr(cdr(args))->type != NULL_TYPE) {
        printf("< statement has too many arguments: expected 2, given %i\n", length(args));
        printf("Expression: (< ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *first = car(args);
    Value *second = car(cdr(args));
    if (isInteger(first)) {
        if (isInteger(second)) {
            return makeBool(first->i < second->i);
        } else if (isDouble(second)) {
            return makeBool(first->i < second->d);
        } else {
            printf("Expected number in <\n");
            printf("Given: ");
            printValue(second);
            printf("\n");
            printf("At expression: (< ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else if (isDouble(first)) {
        if (isInteger(second)) {
            return makeBool(first->d < second->i);
        } else if (isDouble(second)) {
            return makeBool(first->d < second->d);
        } else {
            printf("Expected number in <\n");
            printf("Given: ");
            printValue(second);
            printf("\n");
            printf("At expression: (< ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else {
        printf("Expected number in <\n");
        printf("Given: ");
        printValue(first);
        printf("\n");
        printf("At expression: (< ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    assert(false && "Internal error: should have returned or texited out of if/else block but did not");
}

Value *primitiveGreaterThan(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("> expression has no arguments: expected 2, given none.\n");
        texit(1);
    }
    if (cdr(args)->type != CONS_TYPE) {
        printf("> expression has too few arguments: expected 2, given 1\n");
        printf("Expression: (> ");
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (cdr(cdr(args))->type != NULL_TYPE) {
        printf("> statement has too many arguments: expected 2, given %i\n", length(args));
        printf("Expression: (> ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *first = car(args);
    Value *second = car(cdr(args));
    if (isInteger(first)) {
        if (isInteger(second)) {
            return makeBool(first->i > second->i);
        } else if (isDouble(second)) {
            return makeBool(first->i > second->d);
        } else {
            printf("Expected number in >\n");
            printf("Given: ");
            printValue(second);
            printf("\n");
            printf("At expression: (> ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else if (isDouble(first)) {
        if (isInteger(second)) {
            return makeBool(first->d > second->i);
        } else if (isDouble(second)) {
            return makeBool(first->d > second->d);
        } else {
            printf("Expected number in >\n");
            printf("Given: ");
            printValue(second);
            printf("\n");
            printf("At expression: (> ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else {
        printf("Expected number in >\n");
        printf("Given: ");
        printValue(first);
        printf("\n");
        printf("At expression: (> ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    assert(false && "Internal error: should have returned or texited out of if/else block but did not");
}

Value *primitiveModulo(Value *args) {
    if (args->type != CONS_TYPE) {
        printf("modulo expression has no arguments: expected 2, given none.\n");
        texit(1);
    }
    if (cdr(args)->type != CONS_TYPE) {
        printf("modulo expression has too few arguments: expected 2, given 1\n");
        printf("Expression: (modulo ");
        printTree(args);
        printf(")\n");
        texit(1);
    }
    if (cdr(cdr(args))->type != NULL_TYPE) {
        printf("modulo statement has too many arguments: expected 2, given %i\n", length(args));
        printf("Expression: (modulo ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    Value *first = car(args);
    Value *second = car(cdr(args));
    if (isInteger(first)) {
        if (isInteger(second)) {
            return makeInt(first->i % second->i);
        } else {
            printf("Expected integer in modulo\n");
            printf("Given: ");
            printValue(second);
            printf("\n");
            printf("At expression: (modulo ");
            printTree(args);
            printf(")\n");
            texit(1);
        }
    } else {
        printf("Expected integer in modulo\n");
        printf("Given: ");
        printValue(first);
        printf("\n");
        printf("At expression: (modulo ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    assert(false && "Reached end of primitiveModulo without returning\n");
}

Value *primitiveNot(Value *args) {
    enforceArgumentArity(args, 1, "not");


    if (car(args)->type != BOOL_TYPE) {
        printf("not expression expected boolean.\n");
        printf("Given: ");
        printValue(car(args));
        printf("\n");
        printf("Expression: (not ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    bool argValue = car(args)->i;
    return makeBool(!argValue);
}

void bindPrimitive(char *name, Value *(*function)(struct Value *), Frame *frame) {
    // Add primitive functions to top-level bindings list
    Value *symbol = makeSymbol(name);

    Value *value = makeValue(PRIMITIVE_TYPE);
    value->pf = function;

	Value *binding = makeNull();
	binding = cons(value, binding);
	binding = cons(symbol, binding);
	frame->bindings = cons(binding, frame->bindings);
}

//============================
// Standard Evaluation: apply
//============================

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
            return search;
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
        printf("Given: ");
        printValue(name);
        printf("\n");
        printf("At binding: ");
        printValue(bindingPair);
        printf("\n");
        texit(1);
    }

    // Check that this symbol isn't already bound in the current frame
    Value *bindingValue = lookupBindingInFrame(name, activeFrame);
    if (bindingValue != NULL) {
        // If you're in the second pass of letrec, this is OK.
        // Otherwise, except.
        if (car(bindingValue)->type != UNINITIALIZED) {
            printf("Duplicate binding in one let statement.\n");
            printf("At binding: ");
            printValue(bindingPair);
            printf(";\n");
            printf("For symbol: %s\n", name->s);
            texit(1);
        }
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
        assert(false);
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

//==================
// Special Forms
//==================

Value *evalBegin(Value *argsTree, Frame *activeFrame) {
    Value *result = makeVoid();
    Value *currentExpr = argsTree;
    while(currentExpr->type != NULL_TYPE) {
        result = eval(currentExpr, activeFrame);
        currentExpr = cdr(currentExpr);
    }

    return result;
}

Value *evalCond(Value *argsTree, Frame *activeFrame) {
    // Check for correct syntax with else
    Value *currentExpr = argsTree;
    while (currentExpr->type != NULL_TYPE) {
        Value *condition = car(currentExpr);
        Value *body = cdr(car(currentExpr));

        // Check for else special case
        if (car(condition)->type == SYMBOL_TYPE) {
            if (!strcmp(car(condition)->s, "else")) {
                if (body->type == NULL_TYPE) {
                    printf("Else clause must have a body.\n");
                    printf("At expression: ");
                    printTree(currentExpr);
                    printf("\n");
                    texit(1);
                }
                if (cdr(currentExpr)->type != NULL_TYPE) {
                    printf("Else clause must be last; given too many.\n");
                    printf("At expression: ");
                    printTree(cdr(currentExpr));
                    printf("\n");
                    texit(1);
                }
            }
        }
        currentExpr = cdr(currentExpr);
    }

    // Actually evaluate
    currentExpr = argsTree;
    while (currentExpr->type != NULL_TYPE) {
        Value *condition = car(currentExpr);
        Value *body = cdr(car(currentExpr));

        // Check for else special case
        if (car(condition)->type == SYMBOL_TYPE) {
            if (!strcmp(car(condition)->s, "else")) {
                return evalBegin(body, activeFrame);
            }
        }

        Value *conditionResult = eval(condition, activeFrame);
        if (conditionResult->type != BOOL_TYPE) {
            printf("Condition of cond expression must evaluate to boolean.\n");
            printf("Expression: (cond ");
            printTree(argsTree);
            printf(")\n");
            printf("Condition expression: ");
            printValue(condition);
            printf("\n");
            texit(1);
        }
        if (conditionResult->i) {
            return evalBegin(body, activeFrame);
        }

        currentExpr = cdr(currentExpr);
    }

    // Return void if none of the conditions are true
    return makeVoid();
}

Value *evalAnd(Value *argsTree, Frame *activeFrame) {
    // Evaluates to #t until it hits a false case
    Value *currentExpr = argsTree;
    while(currentExpr->type != NULL_TYPE) {
        Value *condition = eval(currentExpr, activeFrame);
        if (condition->type != BOOL_TYPE) {
            printf("and expression expected boolean.\n");
            printf("Given: ");
            printValue(car(currentExpr));
            printf(", which evaluates to ");
            printValue(condition);
            printf("\n");
            printf("Expression: (and ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }
        if (!condition->i) {
            return makeBool(false);
        }
        currentExpr = cdr(currentExpr);
    }
    return makeBool(true);
}

Value *evalOr(Value *argsTree, Frame *activeFrame) {
    // Evaluates to #f until it hits a true case

    Value *currentExpr = argsTree;
    while(currentExpr->type != NULL_TYPE) {
        Value *condition = eval(currentExpr, activeFrame);
        if (condition->type != BOOL_TYPE) {
            printf("or expression expected boolean.\n");
            printf("Given: ");
            printValue(car(currentExpr));
            printf(", which evaluates to ");
            printValue(condition);
            printf("\n");
            printf("Expression: (or ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }
        if (condition->i) {
            return makeBool(true);
        }
        currentExpr = cdr(currentExpr);
    }

    return makeBool(false);
}

Value *evalDisplay(Value *argTree, Frame *activeFrame) {
    enforceArgumentArity(argTree, 1, "display");

    // Evaluate the argument to display it
    Value *evalResult = eval(argTree, activeFrame);
    printValue(evalResult);

    Value *result = makeValue(VOID_TYPE);
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
        return makeVoid();
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
        return makeVoid();
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

    if (cond->i) {
        // Condition is true
        return eval(thenExpr, activeFrame);
    } else {
        return eval(elseExpr, activeFrame);
    }
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

Value *evalLetStar(Value *argsTree, Frame *activeFrame)  {
    assert(argsTree != NULL);
    assert(activeFrame != NULL);
    if (argsTree->type != CONS_TYPE) {
        printf("let* statement has no body; expected one.\n");
        printf("At expression: (let*)\n");
        texit(1);
    }
    if (cdr(argsTree)->type == NULL_TYPE) {
        printf("let* statement has no body; expected one.\n");
        printf("At expression: (let* ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    if (car(argsTree)->type != CONS_TYPE && car(argsTree)->type != NULL_TYPE) {
        printf("Bindings in let* statement is not a list.\n");
        printf("At expression: (let* ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Frame *letFrame = activeFrame;

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

        Frame *newFrame = talloc(sizeof(Frame));
        newFrame->parent = letFrame;
        newFrame->bindings = makeNull();

        letFrame = makeBinding(car(currentBindingPair), newFrame);
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

Value *evalLetRec(Value *argsTree, Frame *activeFrame)  {
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

    // Make bindings: first pass
    Value *uninitializedValue = makeValue(UNINITIALIZED);

    Value *currentBindingPair = car(argsTree);
    while (currentBindingPair->type != NULL_TYPE) {
        if (car(currentBindingPair)->type != CONS_TYPE && cdr(car(currentBindingPair))->type != CONS_TYPE) {
            printf("Binding in let statement is not a pair.\n");
            printf("At expression: (let ");
            printTree(argsTree);
            printf(")\n");
            printf("At token: ");
            printValue(currentBindingPair);
            printf("\n");
            texit(1);
        }

        // Construct a binding pair to pass to makeBinding
        Value *tempBinding = cons(uninitializedValue, makeNull());
        tempBinding = cons(car(car(currentBindingPair)), tempBinding);
        letFrame = makeBinding(tempBinding, letFrame);

        currentBindingPair = cdr(currentBindingPair);
    }

    // Second pass: for real this time.
    // Letrec 2: Electric Boogaloo
    currentBindingPair = car(argsTree);
    while (currentBindingPair->type != NULL_TYPE) {
        Value *name = car(car(currentBindingPair));
        Value *expr = cdr(car(currentBindingPair));
        assert(car(lookUpSymbol(name, letFrame))->type == UNINITIALIZED);

        Value *exprResult = eval(expr, letFrame);
        if (exprResult->type == UNINITIALIZED) {
            printf("Cannot bind to uninitialized value.\n");
            printf("Binding: ");
            printValue(currentBindingPair);
            printf("\n");
            printf("Expression: (letrec ");
            printTree(argsTree);
            printf(")\n");
            texit(1);
        }

        Value *currentBinding = lookUpSymbol(name, letFrame);
        currentBinding->c.car = exprResult;

        currentBindingPair = cdr(currentBindingPair);
    }

    Value *body = cdr(argsTree);
    return evalBegin(body, letFrame);
}

Value *evalQuote(Value *argsTree, Frame *activeFrame) {
    assert(argsTree != NULL);
    enforceArgumentArity(argsTree, 1, "quote");

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

    Value *closure = makeValue(CLOSURE_TYPE);
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
    // Lambda shorthand syntax
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
    // Standard define
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

    Frame *globalFrame = getGlobalFrame(activeFrame);

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

    return makeVoid();
}

Value *evalSetBang(Value *argsTree, Frame *activeFrame) {
    // Table setting: same as define behavior
    assert(argsTree != NULL);
    if (argsTree->type != CONS_TYPE) {
        printf("Set! statement has no body: expected 2 arguments, given none.\n");
        texit(1);
    }

    if (cdr(argsTree)->type == NULL_TYPE) {
        printf("Set! statement has no expression to bind to.\n");
        printf("Found one argument; expected two.\n");
        printf("At expression: (set! ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *symbol = car(argsTree);
    Value *expr = cdr(argsTree);

    if (car(argsTree)->type != SYMBOL_TYPE) {
        printf("Set! must bind a value to symbol; wrong token type found for symbol name.\n");
        printf("At expression: (set! ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }
    if (cdr(expr)->type != NULL_TYPE) {
        printf("Set! statement has too many arguments: expected 2, given %i\n", length(argsTree));
        printf("Expression: (set! ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    }

    Value *exprResult = eval(expr, activeFrame);

    // Lookup and redefine symbol in active environment
    Value *currentBinding = lookUpSymbol(symbol, activeFrame);
    if (currentBinding == NULL) {
        // Not already bound: this is an error
        printf("Error: cannot set variable before its definition.\n");
        printf("At: (set! ");
        printTree(argsTree);
        printf(")\n");
        texit(1);
    } else {
        // Binding already exists
        // Set the value this binding points to to the new result
        currentBinding->c.car = exprResult;
    }

    return makeVoid();
}

Value *evalLoad(Value *args, Frame *activeFrame) {
    enforceArgumentArity(args, 1, "load");

    Value *filePath = eval(args, activeFrame);

    if (car(args)->type != STR_TYPE) {
        printf("load needs a string file path.\n");
        printf("Given wrong type. \n");
        printf("Expression: (load ");
        printTree(args);
        printf(")\n");
        texit(1);
    }

    FILE *fp = fopen(filePath->s, "r");
    if (fp == NULL) {
        printf("File not found: %s\n", filePath->s);
        texit(1);
    }

    Value *tokenList = tokenize(fp);
    Value *tree = parse(tokenList);
    fclose(fp);

    Frame *globalFrame = getGlobalFrame(activeFrame);

    return evalBegin(tree, globalFrame);
}

//=======================================================
// Fundamentals: Base Function and Expression Evaluation
//=======================================================

Value *eval(Value *tree, Frame *frame) {
    assert(tree != NULL);

    Value *expr = car(tree);

    // Primitive (atomic) types
    if (isInteger(expr)) {
        return expr;
    } else if (isDouble(expr)) {
        return expr;
    } else if (expr->type == STR_TYPE) {
        return expr;
    } else if (isBoolean(expr)) {
        return expr;
    } else if (expr->type == PRIMITIVE_TYPE) {
        return expr;
    } else if (expr->type == NULL_TYPE) {
        return expr;
    } else if (expr->type == SYMBOL_TYPE) {
        Value *binding = lookUpSymbol(expr, frame);
        return car(binding); // Gets value from binding
    } else if (expr->type == UNINITIALIZED) {
        return expr;
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
            } else if (!strcmp(first->s, "let*")) {
                return evalLetStar(args, frame);
            } else if (!strcmp(first->s, "letrec")) {
                return evalLetRec(args, frame);
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
            } else if (!strcmp(first->s, "set!")) {
                return evalSetBang(args, frame);
            } else if (!strcmp(first->s, "begin")) {
                return evalBegin(args, frame);
            } else if (!strcmp(first->s, "cond")) {
                return evalCond(args, frame);
            } else if (!strcmp(first->s, "and")) {
                return evalAnd(args, frame);
            } else if (!strcmp(first->s, "or")) {
                return evalOr(args, frame);
            } else if (!strcmp(first->s, "load")) {
                return evalLoad(args, frame);
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


void interpret(Value *tree) {
    Frame *global = talloc(sizeof(Frame));
    global->parent = NULL;
    global->bindings = makeNull();

	bindPrimitive("+", primitiveAdd, global);
	bindPrimitive("-", primitiveSubtract, global);
	bindPrimitive("*", primitiveMult, global);
	bindPrimitive("/", primitiveDivide, global);
    bindPrimitive("null?", primitiveIsNull, global);
    bindPrimitive("list?", primitiveIsList, global);
    bindPrimitive("car", primitiveCar, global);
    bindPrimitive("cdr", primitiveCdr, global);
    bindPrimitive("cons", primitiveCons, global);
    bindPrimitive("list", primitiveList, global);
    bindPrimitive("append", primitiveAppend, global);
    bindPrimitive("reverse", primitiveReverse, global);
    bindPrimitive("length", primitiveLength, global);
    bindPrimitive("equal?", primitiveEqual, global);
    bindPrimitive("eq?", primitiveEq, global);
    bindPrimitive("number?", primitiveIsNumber, global);
    bindPrimitive("=", primitiveEqualNum, global);
    bindPrimitive("<", primitiveLessThan, global);
    bindPrimitive(">", primitiveGreaterThan, global);
    bindPrimitive("modulo", primitiveModulo, global);
    bindPrimitive("not", primitiveNot, global);

    Value *current = tree;
    while(current->type != NULL_TYPE) {
        Value *result = eval(current, global);

        if (result->type != VOID_TYPE) {
            printValue(result);
            printf("\n");
        }

        current = cdr(current);
    }
    printf("\n");
}