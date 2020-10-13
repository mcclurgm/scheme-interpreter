#include <stdio.h>
#include "tokenizer.h"
#include "value.h"
#include "linkedlist.h"
#include "parser.h"
#include "talloc.h"
#include "interpreter.h"

void evaluate(Frame *global) {
    Value *list = tokenize(stdin);
    Value *tree = parse(list);
    interpret(global, tree);
}

int main(int argc, char *argv[]) {

    Frame *global = initialize();

    evaluate(global);

    tfree();
    return 0;
}
