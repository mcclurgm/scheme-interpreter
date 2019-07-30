CC = clang
CFLAGS = -g
#DEBUG = -DBINARYDEBUG

#SRCS = linkedlist.c value.c talloc.c tokenizer.c parser.c interpreter.c main.c
SRCS = $(wildcard */*.c)
#HDRS = linkedlist.h value.h talloc.h tokenizer.h parser.h interpreter.h
HDRS = $(wildcard */*.h)
OBJS = $(SRCS:.c=.o)

interpreter: $(OBJS)
	$(CC) -rdynamic $(CFLAGS) $^  -o $@

%.o : %.c $(HDRS)
	$(CC)  $(CFLAGS) $(DEBUG) -c $<  -o $@

clean:
	rm *.o
	rm interpreter

