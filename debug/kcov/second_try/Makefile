CC=$(HOME)/gcc-6.1.0/install/bin/gcc
CFLAGS=-g -ggdb -fsanitize-coverage=trace-pc
LIBS=
DEPS=
OBJ=test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

test : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o test
