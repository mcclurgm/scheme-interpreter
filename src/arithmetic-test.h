/* For testing purposes only!!! Many of these functions should not be exposed
 * in a real arithmetic.h file and are internal only.
 */

#ifndef _ARITHMETIC
#define _ARITHMETIC

#include "value.h"

typedef enum {NUMBER_INTEGER, NUMBER_RATIONAL,
              NUMBER_REAL, NUMBER_COMPLEX} numericType;

/* Gets the corresponding numericType for a numeric Value.
 *
 * Requires that the Value passed to it is a number. It is an error to pass
 * another type, and this function will fail and exit the program.
 */
numericType getNumericType(Value *val);

/* Resolves the type of Value to be returned by an arithmetic expression.
 *
 * Each expression must return a single type. The type to be returned follows
 * a hierarchical structure.
 * It uses the least "specific" type of number passed to it, following the
 * hierarchy of numberic types. An integer is a type of rational, which is a
 * type of real, which is a type of complex. The least specific type is the
 * lowest type on this list. For example, 1 + 2.3 contains an integer and a
 * real. Since real is lower on the hierarchy (an integer is a type of real),
 * the resulting type would be real.
 *
 * The type is also exactness preserving. Given any inexact number(s), it
 * returns an inexact number of the least specific type. Given two exact
 * numbers, it returns an exact number of the least specific type.
 * Complex numbers can be either exact or inexact, depending on the exactness
 * of their components.
 *
 * Currently, only integer and real types are implemented.
 */
valueType getResultType(Value *a, Value *b);

// Value *add(Value *a, Value *b);

// Value *subtract(Value *a, Value *b);

// Value *multiply(Value *a, Value *b);

// Value *divide(Value *a, Value *b);

#endif