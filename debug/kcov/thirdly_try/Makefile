CC=$(HOME)/gcc-6.1.0/install/bin/gcc
CFLAGS=-g -ggdb -fsanitize-coverage=trace-pc
LIBS=
DEPS=trace.h
OBJ=test.o trace.o

all: test

trace.o: trace.c
	$(CC) -c -o $@ $<

test.o: test.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

test : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o test
