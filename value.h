#ifndef _VALUE
#define _VALUE

#include <stdbool.h>

typedef enum {INT_TYPE,DOUBLE_TYPE,STR_TYPE,CONS_TYPE,NULL_TYPE,PTR_TYPE,
              OPEN_TYPE,CLOSE_TYPE,BOOL_TYPE,SYMBOL_TYPE, 
              DOT_TYPE, OPEN_BRACKET_TYPE, CLOSE_BRACKET_TYPE, QUOTE_TYPE,
              VOID_TYPE} valueType;

struct Value {
    valueType type;
    bool marked;
    union {
        int i;
        double d;
        char *s;
        void *p;
        struct ConsCell {
            struct Value *car;
            struct Value *cdr;
        } c;
    };
};


typedef struct Value Value;

#endif
