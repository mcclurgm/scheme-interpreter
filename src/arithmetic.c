#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "arithmetic.h"
#include "parser.h"
#include "talloc.h"
#include "number.h"

/* Defines the hierarchical order of numeric types.
 *
 * This hierarchy is as follows:
 *   1. Integer
 *   2. Rational
 *   3. Real
 *   4. Complex
 * In words: an integer is a type of rational, which is a type of real,
 * which is a type of complex.
 *
 * This hierarchy can be used to determine conversion logic. Any number can be
 * converted to a another type if the result is a type of the original number.
 * For instance, an integer can be converted to a rational, since a rational
 * number is a type of integer.
 * In other words, any type higher in the hierarchy can be converted to any type
 * that is lower in the hierarchy. And of course, any type can be converted to
 * itself.
 *
 * This hierarchy is implemented as a C standard enum. It uses the fact that an
 * enum is really an ordered collection of ints. It depends on the fact that the
 * order is rigidly defined to increment up. This behavior is C standard, so
 * should be dependable.
 *
 * In this context, a type's position in the list corresponds to its position in
 * the hierarchy. A type earlier in the list is higher in the hierarchy, and can
 * be converted to any lower (ie later) type.
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
    assert(false && "Unreachable code reached");
    return -1;
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
    assert(false && "Unreachable code reached");
    return -1;
}

/* Resolves the type of number to be returned by an arithmetic expression.
 *
 * This type is represented as a numericType. It cannot be directly used as
 * a valueType.
 *
 * Each expression must return a single type. The type to be returned follows
 * the numericType hierarchical structure.
 *
 * It uses the lowest type of number passed to it, following the hierarchy.
 * An integer is a type of rational, which is a type of real, which is a type of
 * complex. For example, 1 + 2.3 contains an integer and a real. Since real is
 * lower on the hierarchy (an integer is a type of real), the resulting type
 * would be real.
 *
 * The type is also exactness preserving. Given any inexact number(s), it
 * returns an inexact number of the lowest hierarchical type. Given two exact
 * numbers, it returns an exact number of the least specific type.
 * Note complex numbers can be either exact or inexact, depending on the
 * exactness of their components.
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
 *
 * a and b are numeric-typed Values.
 */
Value *add(Value *a, Value *b) {
    numericType resultType = getResultType(a, b);

    // Convert numbers to the same type to be operated on
    // (reusable for all arithmetic)
    Value *convertedA;
    Value *convertedB;
    if (resultType == NUMBER_INTEGER) {
        convertedA = convertInteger(a);
        convertedB = convertInteger(b);
    } else if (resultType == NUMBER_REAL) {
        convertedA = convertReal(a);
        convertedB = convertReal(b);
    } else {
        assert(false && "Numeric type that is invalid or not implemented");
    }

    // Perform the operation and return
    // (unique to addition)
    if (resultType == NUMBER_INTEGER) {
        return intAdd(convertedA, convertedB);
    } else if (resultType == NUMBER_REAL) {
        return realAdd(convertedA, convertedB);
    } else {
        printf("INTERNAL ERROR.\n");
        printf("It seems like we're trying to add two numbers, \n");
        printf("and they have an invalid or not implemented result type.\n");
        texit(2);
    }
    assert(false && "Unreachable code reached");
    return makeNull();
}

/* Calculates the result of a - b.
 */
Value *subtract(Value *a, Value *b) {
    numericType resultType = getResultType(a, b);

    // Convert numbers to the same type to be operated on
    Value *convertedA;
    Value *convertedB;
    if (resultType == NUMBER_INTEGER) {
        convertedA = convertInteger(a);
        convertedB = convertInteger(b);
    } else if (resultType == NUMBER_REAL) {
        convertedA = convertReal(a);
        convertedB = convertReal(b);
    } else {
        assert(false && "Numeric type that is invalid or not implemented");
    }

    // Perform the operation and return
    // (unique to addition)
    if (resultType == NUMBER_INTEGER) {
        return intSubtract(convertedA, convertedB);
    } else if (resultType == NUMBER_REAL) {
        return realSubtract(convertedA, convertedB);
    } else {
        printf("INTERNAL ERROR.\n");
        printf("It seems like we're trying to add two numbers, \n");
        printf("and they have an invalid or not implemented result type.\n");
        texit(2);
    }
    assert(false && "Unreachable code reached");
    return makeNull();
}

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