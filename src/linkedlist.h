#ifndef _LINKEDLIST
#define _LINKEDLIST

#include <stdbool.h>
#include "value.h"

// Create a new CONS_TYPE value node.
//
// FAQ: If the newCdr contains a string within, do we need to make a copy of
// that string?
//
// ANS: Yes, you should make a copy of that string. The function cleanup (below)
// is then responsible for freeing the memory associated with this copy of the
// string.
Value *cons(Value *newCar, Value *newCdr);

// Display the contents of the linked list to the screen in some kind of
// readable format
void display(Value *list);

// Return a new list that is the reverse of the one that is passed in. All
// content within the list should be duplicated; there should be no shared
// memory between the original list and the new one.
//
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
Value *reverse(Value *list);

// Frees up all memory directly or indirectly referred to by list.
// FAQ: What if there are nested lists inside that list?
// ANS: There won't be for this assignment. There will be later, but that will
// be after we've got an easier way of managing memory.
void cleanup(Value *list);

// Utility to make it less typing to get car value. Use assertions to make sure
// that this is a legitimate operation.
Value *car(Value *list);

// Utility to make it less typing to get cdr value. Use assertions to make sure
// that this is a legitimate operation.
Value *cdr(Value *list);

// Utility to check if pointing to a NULL_TYPE value. Use assertions to make sure
// that this is a legitimate operation.
bool isNull(Value *value);

// Measure length of list. Use assertions to make sure that this is a legitimate
// operation.
int length(Value *value);

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
Value *append(Value *newList, Value *oldList);

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
Value *list(int dim, Value *values[]);

#endif
