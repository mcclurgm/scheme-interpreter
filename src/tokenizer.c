#include "tokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

void catLetter(char *str, char letter) {
    int strLength = strlen(str);
    str[strLength] = letter;
    str[strLength +1] = '\0';
    assert(strLength+1 ==  strlen(str) && "catLetter produces wrong length string");
}

bool isSymbolInitial(char c) {
    if (isalpha(c)) {
        return true;
    } else if (c == '!') {
        return true;
    } else if (c == '$') {
        return true;
    } else if (c == '%') {
        return true;
    } else if (c == '&') {
        return true;
    } else if (c == '*') {
        return true;
    } else if (c == '/') {
        return true;
    } else if (c == ':') {
        return true;
    } else if (c == '<') {
        return true;
    } else if (c == '=') {
        return true;
    } else if (c == '>') {
        return true;
    } else if (c == '?') {
        return true;
    } else if (c == '~') {
        return true;
    } else if (c == '_') {
        return true;
    } else if (c == '^') {
        return true;
    } else {
        return false;
    }
}

bool isSymbolSubsequent(char c) {
    if (isSymbolInitial(c)) {
        return true;
    } else if (isdigit(c)) {
        return true;
    } else if (c == '+') {
        return true;
    } else if (c == '-') {
        return true;
    } else if (c == '.') {
        return true;
    } else {
        return false;
    }
}

Value *endToken(Value *tokens) {
    char *tokenString = car(tokens)->s;
    if (isInteger(car(tokens))) {
        car(tokens)->i = atoi(tokenString);
    }
    else if (isDouble(car(tokens))) {
        car(tokens)->d = atof(tokenString);
    }
    else if (isBoolean(car(tokens))) {
        if (!strcmp(tokenString, "#t")) {
            car(tokens)->i = true;
        } else if (!strcmp(tokenString, "#f")) {
            car(tokens)->i = false;
        } else {
            // Invalid boolean.
            // Trying to end token before it reaches multiple characters:
            // just "#". Other cases (invalid character, too long, etc) should
            // be caught in parseBool.
            assert(!strcmp(tokenString, "#t") && "Invalid token made it past parseBool");
            printf("Syntax error: invalid bool declaration.\n");
            printf("Isolated '#' is invalid.\n");
            texit(1);
        }
    }
    tokens = cons(makeNull(), tokens);
    return tokens;
}

Value *parseParens(char charRead, Value *tokens) {
    if (charRead == '(') {
        if (!isNull(car(tokens))) {
            tokens = endToken(tokens);
        }
        car(tokens)->type = OPEN_TYPE;
        car(tokens)->s = talloc(2 * sizeof(char));
        strcpy(car(tokens)->s, "(");

        // End the current token
        tokens  = endToken(tokens);
    } else if (charRead == ')') {
        if (!isNull(car(tokens))) {
            tokens = endToken(tokens);
        }
        car(tokens)->type = CLOSE_TYPE;
        car(tokens)->s = talloc(2 * sizeof(char));
        strcpy(car(tokens)->s, ")");

        // End the current token
        tokens = endToken(tokens);
    } else if (charRead == '[') {
        if (!isNull(car(tokens))) {
            tokens = endToken(tokens);
        }
        car(tokens)->type = OPEN_BRACKET_TYPE;
        car(tokens)->s = talloc(2 * sizeof(char));
        strcpy(car(tokens)->s, "[");

        // End the current token
        tokens = endToken(tokens);
    } else if (charRead == ']') {
        if (!isNull(car(tokens))) {
            tokens = endToken(tokens);
        }
        car(tokens)->type = CLOSE_BRACKET_TYPE;
        car(tokens)->s = talloc(2 * sizeof(char));
        strcpy(car(tokens)->s, "]");

        // End the current token
        tokens = endToken(tokens);
    }
    return tokens;
}

Value *parseNumber(char charRead, Value *tokens) {
    valueType tokenType = car(tokens)->type;
    if (tokenType == INT_TYPE) {
        if (charRead == '.') {
            car(tokens)->type = DOUBLE_TYPE;
            catLetter(car(tokens)->s, charRead);
        } else if (isdigit(charRead)) {
            catLetter(car(tokens)->s, charRead);
        } else {
            printf("Syntax error.\n");
            catLetter(car(tokens)->s, charRead);
            printf("Unparseable number: %s\n", car(tokens)->s);
            texit(1);
        }
    }
    else if (tokenType == DOUBLE_TYPE) {
        if (isdigit(charRead)) {
            catLetter(car(tokens)->s, charRead);
        }
        else {
            printf("Syntax error.\n");
            catLetter(car(tokens)->s, charRead);
            printf("Unparseable number: %s\n", car(tokens)->s);
            texit(1);
        }
    }

    return tokens;
}

Value *parseSymbol(char charRead, Value *tokens) {
    if (strcmp(car(tokens)->s, "+") == 0 || strcmp(car(tokens)->s, "-") == 0) {
        // These cases create numbers, not symbols
        if (isdigit(charRead)) {
            car(tokens)->type = INT_TYPE;
            catLetter(car(tokens)->s, charRead);
        } else if (charRead == '.') {
            car(tokens)->type = DOUBLE_TYPE;
            catLetter(car(tokens)->s, charRead);
        } else {
            printf("Syntax error: invalid symbol.\n");
            catLetter(car(tokens)->s, charRead);
            printf("Entered: %s\n", car(tokens)->s);
            texit(1);
        }
    } else if (isSymbolSubsequent(charRead)) {
        catLetter(car(tokens)->s, charRead);
    } else {
        printf("Syntax error: invalid symbol.\n");
        catLetter(car(tokens)->s, charRead);
        printf("Entered: %s\n", car(tokens)->s);
        texit(1);
    }

    return tokens;
}

Value *parseBool(char charRead, Value *tokens) {
    // Note: this assumes that it is appending to an existing bool (which must
    // start with '#'), and can't handle a first character.
    if (!strcmp(car(tokens)->s, "#")) {
        if (charRead == 't' || charRead == 'f') {
            catLetter(car(tokens)->s, charRead);
        } else {
            printf("Syntax error. Invalid character in Boolean declaration at char:\n");
            printf("%c\n", charRead);
            texit(1);
        }
    } else {
        catLetter(car(tokens)->s, charRead);
        printf("Syntax error. Invalid Boolean declaration: %s at char: ", car(tokens)->s);
        printf("%c\n", charRead);
        texit(1);
    }
    return tokens;
}

Value *parseDot(char charRead, Value *tokens) {
    if (isdigit(charRead)) {
        car(tokens)->type = DOUBLE_TYPE;
        catLetter(car(tokens)->s, charRead);
    } else {
        printf("Syntax error: dot must be on its own or in number.\n");
        catLetter(car(tokens)->s, charRead);
        printf("At token: %s\n", car(tokens)->s);
        texit(1);
    }
    return tokens;
}

Value *parseFirstChar(char charRead, Value *tokens) {
    if (isdigit(charRead)) {
        car(tokens)->type = INT_TYPE;
        car(tokens)->s = talloc(255*sizeof(char));
        strcpy(car(tokens)->s, "");
        catLetter(car(tokens)->s, charRead);
    }
    // Change for Bonus?
    else if (charRead == '.') {
        car(tokens)->type = DOT_TYPE;
        car(tokens)->s = talloc(255*sizeof(char));
        strcpy(car(tokens)->s, ".");
    }
    else if (charRead == '#') {
        car(tokens)->type = BOOL_TYPE;
        car(tokens)->s = talloc(255*sizeof(char));
        strcpy(car(tokens)->s, "#");
    }
    else if (isSymbolInitial(charRead) || charRead == '+' || charRead == '-') {
        car(tokens)->type = SYMBOL_TYPE;
        car(tokens)->s = talloc(255*sizeof(char));
        strcpy(car(tokens)->s, "");
        catLetter(car(tokens)->s, charRead);
    } else if (!isspace(charRead)) {
        printf("Syntax error: invalid character %c\n", charRead);
        texit(1);
    }

    return tokens;
}

// Read all of the input from the file pointer fp, and return a linked list consisting of the
// tokens.
Value *tokenize(FILE *fp) {
    char charRead;
    Value *tokens = makeNull();
    charRead = (char)getc(fp);

    // Initialize the first data cons cell
    tokens = cons(makeNull(), tokens);

    bool inComment = false;
    bool inString = false;
    while (!feof(fp)) {
        if (!isNull(cdr(tokens))) {
            if (isNull(car(cdr(tokens)))) {
                printf("Null type before character: %c\n", charRead);
            }
        }

        // Check state for cases that don't parse for new tokens
        if (inComment) {
            if (charRead == '\n') {
                inComment = false;
            }
        }
        else if (inString) {
            if (charRead == '"') {
                inString = false;
                tokens = endToken(tokens);
            }
            else {
                catLetter(car(tokens)->s, charRead);
            }
        }

        else if (charRead == ';') {
            inComment = true;
            if (!isNull(car(tokens))) {
                tokens = endToken(tokens);
            }
        }
        else if (charRead == '"') {
            inString = true;
            if (!isNull(car(tokens))) {
                tokens = endToken(tokens);
            }
            car(tokens)->type = STR_TYPE;
            car(tokens)->s = talloc(255*sizeof(char));
            strcpy(car(tokens)->s, "");
        }

        // Beginning of token
        else if (charRead == '(' || charRead == ')'
                 || charRead == '[' || charRead == ']') {
            tokens = parseParens(charRead, tokens);
        }
        else if (charRead == '\'') {
            if (!isNull(car(tokens))) {
                tokens = endToken(tokens);
            }
            car(tokens)->type = QUOTE_TYPE;
            car(tokens)->s = talloc(2 * sizeof(char));
            strcpy(car(tokens)->s, "'");
            tokens  = endToken(tokens);
        }

        else if (isNull(car(tokens))) {
            tokens = parseFirstChar(charRead, tokens);
        }

        else if (isspace(charRead)) {
            if (!isNull(car(tokens))) {
                tokens = endToken(tokens);
            }
        }

        // In the middle of an existing token
        else if (isInteger(car(tokens)) || isDouble(car(tokens))) {
            tokens = parseNumber(charRead, tokens);
        }

        else if (isBoolean(car(tokens))) {
            tokens = parseBool(charRead, tokens);
        } else if (isType(car(tokens), DOT_TYPE)) {
            tokens = parseDot(charRead, tokens);
        } else if (isSymbol(car(tokens))) {
            tokens = parseSymbol(charRead, tokens);
        }

        charRead = (char)fgetc(fp);
    }

    // Finalize the last token
    if (!isNull(car(tokens))) {
        tokens = endToken(tokens);
    }
    tokens = cdr(tokens);

    if (inString) {
        printf("Untokenizable. Unclosed string at end of file.\n");
        texit(1);
    }

    Value *revList = reverse(tokens);
    return revList;
}

// Displays the contents of the linked list as tokens, with type information
void displayTokens(Value *list) {
    assert(list != NULL);

    Value *current = list;
    while (!isNull(current)) {
        if (isInteger(car(current))) {
            printf("%i:integer\n", car(current)->i);
        }
        else if (isDouble(car(current))) {
            printf("%f:decimal\n", car(current)->d);
        }
        else if (isString(car(current))) {
            printf("\"%s\":string\n", car(current)->s);
        }
        else if (isSymbol(car(current))) {
            printf("%s:symbol\n", car(current)->s);
        }
        else if (isType(car(current), OPEN_TYPE)) {
            printf("%s:open\n", car(current)->s);
        }
        else if (isType(car(current), CLOSE_TYPE)) {
            printf("%s:close\n", car(current)->s);
        }
        else if (isBoolean(car(current))) {
            if (car(current)->i > 0) {
                printf("#t:bool\n");
            } else {
                printf("#f:bool\n");
            }
        }
        else if (isType(car(current), QUOTE_TYPE)) {
            printf("%s:singlequote\n", car(current)->s);
        }
        else if (isType(car(current), OPEN_BRACKET_TYPE)) {
            printf("%s:openbracket\n", car(current)->s);
        }
        else if (isType(car(current), CLOSE_BRACKET_TYPE)) {
            printf("%s:closebracket\n", car(current)->s);
        }
        else if (isType(car(current), DOT_TYPE)) {
            printf("%s:dot\n", car(current)->s);
        }
        else if (isNull(car(current))) {
            printf("Null type, this should be an ERROR.\n");
        }

        current = cdr(current);
    }
}
