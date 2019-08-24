/* A main file to test the arithmetic system, independent of the surrounding
 * Scheme interpeter system.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "value.h"
#include "arithmetic-test.h"
#include "talloc.h"

int main() {
    Value *a = makeInt(1);
    assert(getNumericType(a) == 0);
    Value *b = makeInt(-100000000);
    assert(getNumericType(b) == 0);
    Value *c = makeDouble(1.05);
    assert(getNumericType(c) == 2);
    // Value *d = makeBool(true);
    // getNumericType(d);
    // assert(false && "You should not have made it here.");
    // Value *e = makeString("Hello there");
    // getNumericType(e);
    // assert(false && "You should not have made it here.");

    assert(getResultType(a, b) == 0);
    assert(getResultType(c, c) == 2);
    assert(getResultType(a, c) == 2);
    assert(getResultType(c, b) == 2);

    // Integer result
    Value *result = add(makeInt(1), makeInt(2));
    assert(result->type == INT_TYPE);
    assert(result->i == 3);

    // Real result
    result = add(makeDouble(1.5), makeDouble(2.5));
    assert(result->type == DOUBLE_TYPE);
    assert(result->d == 4);
    result = add(makeInt(1), makeDouble(2.5));
    assert(result->type == DOUBLE_TYPE);
    assert(result->d == 3.5);
    result = add(makeDouble(2.5), makeInt(1));
    assert(result->type == DOUBLE_TYPE);
    assert(result->d == 3.5);

    printf("Well done\n");
    return 0;
}