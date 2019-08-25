#include <stdio.h>
#include <assert.h>
#include "number.h"
#include "talloc.h"

/* Convert input to an integer-typed Value.
 *
 * This only performs conversions "down" the numeric hierarchy, from one type
 * to another type that is a subset of the original type. Passing a number of a
 * "lower" type (in the case of integers: rational, real, complex) is invalid
 * and an error.
 * Note that in the case of integers, only integers can be converted to
 * integers. This function only accepts integers, and in effect does nothing.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertInteger(Value *val) {
    assert(isNumber(val));
    if (val->type == INT_TYPE) {
        return val;
    } else if (isNumber(val)) {
        printf("INTERNAL ERROR.");
        printf("Attempting to perform an invalid number conversion,\n");
        printf("to a type that is not allowed.\n");
        printf("Converting to integer from lower rank number (valueType %i).\n", val->type);
        texit(2);
    } else {
        assert(false && "Converting a numeric type that is not implemented");
    }
    assert(false && "Unreachable code reached");
    return makeNull();
}

/* Return the result of adding two integers.
 *
 * This function requires that its arguments are both integer typed
 * (ie, INT_TYPE). It does not convert values. It is an error to pass it
 * anything else.
 */
Value *intAdd(Value *a, Value * b) {
    assert(a->type == INT_TYPE && b->type == INT_TYPE &&
           "Wrong type being passed to intAdd: requires integer only.");
    return makeInt(a->i + b->i);
}

/* Return the result of subtracting two integers.
 *
 * This function requires that its arguments are both integer typed
 * (ie, INT_TYPE). It does not convert values. It is an error to pass it
 * anything else.
 */
Value *intSubtract(Value *a, Value * b) {
    assert(a->type == INT_TYPE && b->type == INT_TYPE &&
           "Wrong type being passed to intSubtract: requires integer only.");
    return makeInt(a->i - b->i);
}

/* Convert input to a real-typed Value.
 *
 * This only performs conversions "down" the numeric hierarchy, from one type
 * to another type that is a subset of the original type. Passing a number of a
 * "lower" type (in the case of reals: complex) is invalid and an error.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertReal(Value *val) {
    assert(isNumber(val));
    if (val->type == INT_TYPE) {
        double doubleVal = (double)(val->i);
        return makeDouble(doubleVal);
    } else if (val->type == DOUBLE_TYPE) {
        return val;
    } else if (isNumber(val)) {
        printf("INTERNAL ERROR.");
        printf("Attempting to perform an invalid number conversion,\n");
        printf("to a type that is not allowed.\n");
        printf("Converting to real from lower rank number (valueType %i).\n", val->type);
        texit(2);
    } else {
        assert(false && "Converting a numeric type that is not implemented");
    }
    assert(false && "Unreachable code reached");
    return makeNull();
}

/* Return the result of adding two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realAdd(Value *a, Value *b) {
    assert(a->type == DOUBLE_TYPE && b->type == DOUBLE_TYPE &&
           "Wrong type being passed to realAdd: requires real only.");
    return makeDouble(a->d + b->d);
}

/* Return the result of subtracting two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realSubtract(Value *a, Value *b) {
    assert(a->type == DOUBLE_TYPE && b->type == DOUBLE_TYPE &&
           "Wrong type being passed to realAdd: requires real only.");
    return makeDouble(a->d - b->d);
}
