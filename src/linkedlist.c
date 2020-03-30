// Implementation of LinkedList for Interpreter Project
// James Gardner and Michael McClurg
// CS251 Programming Languages

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "value.h"
#include "linkedlist.h"
#include "talloc.h"
#include "parser.h"


// Create a new CONS_TYPE value node.
//
// FAQ: If the newCdr contains a string within, do we need to make a copy of
// that string?
//
// ANS: Yes, you should make a copy of that string. The function cleanup (below)
// is then responsible for freeing the memory associated with this copy of the
// string.
Value *cons(Value *newCar, Value *newCdr) {
    assert(newCar != NULL);
    assert(newCdr != NULL);

    Value *cell = makeValue(CONS_TYPE);

    if ((*newCar).type == STR_TYPE){
        char* newString = talloc(sizeof(char) * (strlen((*newCar).s)+1));
        strcpy(newString, (*newCar).s);
        (*newCar).s = newString;
    }

    if ((*newCdr).type == STR_TYPE){
        char* newString = talloc(sizeof(char) * (strlen((*newCdr).s)+1));
        strcpy(newString, (*newCdr).s);
        (*newCdr).s = newString;
    }

    (*cell).c.car = newCar;
    (*cell).c.cdr = newCdr;
    return cell;

}

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list) {
    assert(list != NULL);
    if ((*list).type == CONS_TYPE) {
        display((*list).c.car);
        display((*list).c.cdr);
    }
    else if ((*list).type == INT_TYPE) {
        printf("%i\n", (*list).i);
    }
    else if ((*list).type == DOUBLE_TYPE) {
        printf("%f\n", (*list).d);
    }
    else if ((*list).type == STR_TYPE) {
        printf("%s\n", (*list).s);
    }
    else if (isType(list, PTR_TYPE)) {
        printf("%p\n", list->p);
    }
    else if (isType(list, OPEN_TYPE)) {
        printf("%s\n", list->s);
    }
    else if (isType(list, CLOSE_TYPE)) {
        printf("%s\n", list->s);
    }
    else {
        printf("()\n");
    }
}


// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be not duplicated,
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list) {
    assert(list != NULL);
    assert((*list).type == CONS_TYPE || isNull(list));
    if (isProperList(list)) {
        Value *partial = makeNull();
        Value *current = list;
        while ((*current).type != NULL_TYPE) {
            // Build revesred list with cons
            assert((*current).type == CONS_TYPE || isNull(current));
            partial = cons(car(current), partial);
            current = cdr(current);
        }
        return partial;

    } else {
        printf("reverse: contract violation\n");
        printf("expected: list?\n");
        texit(1);
    }
    return list;
}

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.

// Assuming a proper list will be passed, throws exceptions if it's not
int length(Value *value) {
    assert(value != NULL);
    assert((*value).type == CONS_TYPE || isNull(value));

    if (isProperList(value)) {
        Value *current = value;
        int length = 0;
        while ((*current).type != NULL_TYPE) {
            assert((*current).type == CONS_TYPE || isNull(current));
            length++;
            current = cdr(current);
        }
        return length;
    } else {
        printf("length: contract violation\n");
        printf("expected: list?\n");
        texit(1);
    }
    return 0;
}

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list) {
    assert(list != NULL);
    assert((*list).type == CONS_TYPE);
    return (*list).c.car;
}

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list) {
    assert(list != NULL);
    assert((*list).type == CONS_TYPE);
    return (*list).c.cdr;
}


// Appends the elements of newList onto the existing list oldList.
// This mimics the Scheme append function.
// This keeps the references of (aliases) oldList in the Value it returns.
//
// Takes two cons-type Value pointers.
// Returns one cons-type Value pointer.
//
// To test: create two lists, say of length 2 each.
// Then call:
// Value *test = append(list1, list2);
Value *append(Value *newList, Value *oldList) {
    assert(newList != NULL);
    assert(oldList != NULL);
    assert((*newList).type == CONS_TYPE || isNull(newList));
    assert((*oldList).type == CONS_TYPE || isNull(oldList));
    if (isProperList(newList) && isProperList(oldList)) {
        Value *result = oldList;
        Value *current = reverse(newList);
        while ((*current).type != NULL_TYPE) {
            assert((*current).type == CONS_TYPE || isNull(current));
            result = cons(car(current), result);
            current = cdr(current);
        }
        return result;
    } else {
        printf("append: contract violation\n");
        printf("expected: list?\n");
        texit(1);
    }
    return newList;
}

// Creates a linked list from the values in values[].
// This mimics the Scheme function list.
//
// Takes an array of Value pointers.
// These must be of int, double, or string type--not cons or null.
// It also takes the length of values[] as a parameter.
// Returns a cons-type Value pointer.
//
// To test, create multiple Value pointers containing data, not cons cells.
// Make an array of Value pointers and fill it with these pointers.
// Then call
// test = list(3, values);
// (for example, with an array of length 3 to add 3 elements)
Value *list(int dim, Value *values[]) {
    assert(values != NULL);
    Value *result = makeNull();
    for(int i = dim - 1; i >= 0; i--) {
        assert(values[i] != NULL);
        // Assuming NULL_TYPE not accepted because that would be a nested list of sorts
        assert((*values[i]).type != CONS_TYPE && (*values[i]).type != NULL_TYPE);
        result = cons(values[i], result);
    }

    return result;
}
