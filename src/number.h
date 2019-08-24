#ifndef _NUMBER
#define _NUMBER

#include "value.h"

/* Converts its input to an integer-typed Value.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertInteger(Value *val);

/* Return the result of adding two integers.
 *
 * This function requires that its arguments are both integer typed
 * (ie, INT_TYPE). It does not convert values. It is an error to pass it
 * anything else.
 */
Value *intAdd(Value *a, Value * b);

/* Converts its input to a real-typed Value.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertInteger(Value *val);

/* Return the result of adding two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realAdd(Value *a, Value *b);

#endif