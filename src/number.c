#include "number.h"
#include "assert.h"

/* Converts its input to an integer-typed Value.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertInteger(Value *val) {
    assert(isNumber(val));
    if (val->type == INT_TYPE) {
        return val;
    } else if (val->type == DOUBLE_TYPE) {
        double doubleVal = (double)(val->i);
        return makeDouble(doubleVal);
    } else {
        assert(false && "Converting a numeric type that is invalid or not implemented");
    }
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

/* Return the result of adding two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realAdd(Value *a, Value *b) {
    assert(a->type == DOUBLE_TYPE && b->type == DOUBLE_TYPE &&
           "Wrong type being passed to intAdd: requires integer only.");
    return makeDouble(a->d + b->d);
}
