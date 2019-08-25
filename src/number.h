#ifndef _NUMBER
#define _NUMBER

#include "value.h"

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
Value *convertInteger(Value *val);

/* Return the result of adding two integers.
 *
 * This function requires that its arguments are both integer typed
 * (ie, INT_TYPE). It does not convert values. It is an error to pass it
 * anything else.
 */
Value *intAdd(Value *a, Value * b);

/* Return the result of subtracting two integers.
 *
 * This function requires that its arguments are both integer typed
 * (ie, INT_TYPE). It does not convert values. It is an error to pass it
 * anything else.
 */
Value *intSubtract(Value *a, Value * b);

/* Convert input to a real-typed Value.
 *
 * This only performs conversions "down" the numeric hierarchy, from one type
 * to another type that is a subset of the original type. Passing a number of a
 * "lower" type (in the case of reals: complex) is invalid and an error.
 *
 * Requires that its input is of numeric type. It is an error to pass
 * anything else.
 */
Value *convertReal(Value *val);

/* Return the result of adding two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realAdd(Value *a, Value *b);

/* Return the result of subtracting two real numbers.
 *
 * This function requires that its arguments are both real typed
 * (ie, DOUBLE_TYPE). It does not convert values.
 */
Value *realSubtract(Value *a, Value *b);

#endif