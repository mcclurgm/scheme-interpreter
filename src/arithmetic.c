#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "arithmetic.h"
#include "parser.h"
#include "talloc.h"
#include "number.h"

/* Defines the hierarchical order of numeric types.
 * This uses the fact that an enum is really an ordered collection of ints.
 * It depends on the fact that the order is rigidly defined to increment up.
 * This behavior is C standard, so should be dependable.
 *
 * This can be used to determine conversion logic. Any type higher in the list
 * is in a higher position in the hierarchy, and can be implicitly converted
 * down to any lower type.
 *
 * Yes, this duplicates functionality from valueType. But it is a good way to
 * guarantee that the hierarchy is respected and documented.
 */
typedef enum {NUMBER_INTEGER, NUMBER_RATIONAL,
              NUMBER_REAL, NUMBER_COMPLEX} numericType;

/* Gets the corresponding numericType for a numeric Value.
 *
 * Requires that the Value passed to it is a number. It is an error to pass
 * another type, and this function will fail and exit the program.
 */
numericType getNumericType(Value *val) {
    if (isInteger(val)) {
        return NUMBER_INTEGER;
    } else if (isDouble(val)) {
        return NUMBER_REAL;
    } else {
        printf("Given invalid type in arithmetic operation.\n");
        printf("Value ");
        printValue(val);
        printf(" is not a number.\n");
        texit(2);
        //TODO Is this an internal error or an input error?
    }
}

/* Convert a numericType to a valueType. */
valueType getValueType(numericType type) {
    if (type == NUMBER_INTEGER) {
        return INT_TYPE;
    } else if (type == NUMBER_REAL) {
        return DOUBLE_TYPE;
    } else {
        assert(false && "numericType that isn't implemented");
    }
}

/* Resolves the type of number to be returned by an arithmetic expression.
 *
 * This type is represented as a numericType. It cannot be directly used as
 * a valueType.
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
numericType getResultType(Value *a, Value *b) {
    // Get numeric types of inputs
    numericType aType = getNumericType(a);
    numericType bType = getNumericType(b);

    // Get resulting type. This will be the lowest in the hierarchy, ie the
    // smallest number numericType.
    if (aType > bType) {
        return aType;
    } else {
        return bType;
    }
}

/* Calculates the result of a + b.
 */
Value *add(Value *a, Value *b) {
    numericType resultType = getResultType(a, b);

    // Convert numbers to the same type to be operated on
    Value *convertedA;
    Value *convertedB;
    if (resultType == NUMBER_INTEGER) {
        convertedA = convertInteger(a);
        convertedB = convertInteger(b);
    } else {
        assert(false && "Numeric type that is invalid or not implemented");
    }

    if (resultType == NUMBER_INTEGER) {
        return intAdd(a, b);
    } else {
        printf("INTERNAL ERROR.\n");
        printf("It seems like we're trying to add two numbers, and they have an invalid type.\n");
        texit(2);
    }
    return makeNull();
}

/* Calculates the result of a - b.
 */
// Value *subtract(Value *a, Value *b) {
//     return makeNull();
// }

/* Calculates the result of a * b
 */
// Value *multiply(Value *a, Value *b) {
//     return makeNull();
// }

/* Calculates the result of a / b.
 */
// Value *divide(Value *a, Value *b) {
//     return makeNull();
// }