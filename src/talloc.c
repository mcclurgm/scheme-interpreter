#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "value.h"
#include "talloc.h"
#include "linkedlist.h"
#include <stdio.h>

Value *pointers;
int isInit = 0;

// Create a new NULL_TYPE value node.
Value *tallocMakeNull() {
    Value *null = malloc(sizeof(Value));
    (*null).type = NULL_TYPE;
    (*null).marked = false;
    return null;

}

// Create a new CONS_TYPE value node.
//
// FAQ: If the newCdr contains a string within, do we need to make a copy of
// that string?
//
// ANS: Yes, you should make a copy of that string. The function cleanup (below)
// is then responsible for freeing the memory associated with this copy of the
// string.
Value *tallocCons(Value *newCar, Value *newCdr) {
    assert(newCar != NULL);
    assert(newCdr != NULL);
    assert((*newCdr).type == CONS_TYPE || isNull(newCdr));
    assert(newCar->type == PTR_TYPE);

    // Value *car = malloc(sizeof(Value));
    // (*car).marked = false;
    // (*car).type = (*newCar).type;
    // (*car).p = (*newCar).p;
    
    Value *cell = malloc(sizeof(Value));
    (*cell).marked = false;
    (*cell).type = CONS_TYPE;
    (*cell).c.car = newCar;
    (*cell).c.cdr = newCdr;
    return cell;
}

// Replacement for malloc that stores the pointers allocated. It should store
// the pointers in some kind of list; a linked list would do fine, but insert
// here whatever code you'll need to do so; don't call functions in the
// pre-existing linkedlist.h. Otherwise you'll end up with circular
// dependencies, since you're going to modify the linked list to use talloc.

void *talloc(size_t size) {
    void *newPointer = malloc(size);

    if (isInit == 0) {
        pointers = tallocMakeNull();
        isInit = 1;
    }

    Value *consMe = malloc(sizeof(Value));
    consMe->marked = false;
    consMe->type = PTR_TYPE;
    consMe->p = newPointer;
    pointers = tallocCons(consMe, pointers);
    return newPointer;
}

// Free all pointers allocated by talloc, as well as whatever memory you
// allocated in lists to hold those pointers.
void tfree() {
    // Build a new pointers list and use this at the end
    Value *newPointers = tallocMakeNull();
    
    Value *current = pointers;
    while ((*current).type != NULL_TYPE) {
        Value *currentPointer = car(current);
        bool isMarked = currentPointer->marked;
        // printf("tfree marked? %i\n", isMarked);
        if (isMarked) {
            currentPointer->marked = false;
            newPointers = tallocCons(currentPointer, newPointers);
        } else {
            free(car(current)->p);
        }
        free(car(current));
        Value *helper = current;
        current = cdr(current);
        free(helper);
    }
    free(current);
    if (length(newPointers) == 0) {
        free(newPointers);
        isInit = 0;
    } else {
        pointers = newPointers;
    }
}

// Replacement for the C function "exit", that consists of two lines: it calls
// tfree before calling exit. It's useful to have later on; if an error happens,
// you can exit your program, and all memory is automatically cleaned up.
void texit(int status) {
    tfree();
    exit(status);
}

/*TODO: 
We're setting the marked property in the Value passed to this function.
Dave seems to expect that we're marking it in the pointer to the Value (or
whatever it is). How do we deal with this???? 

Looks right to Dave, how it being used may be the issue
*/
void mark(Value *value) {
    assert(value != NULL);
    value->marked = true;
}

