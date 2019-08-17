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

    printf("Well done\n");
    return 0;
}