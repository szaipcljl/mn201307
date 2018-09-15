all: cshell cbin

cshell: cshell.o
	gcc -o cshell cshell.o
cshell.o: cshell.c
	gcc -c cshell.c

cbin: cbin.o
	gcc -o cbin cbin.o
cbin.o: cbin.c
	gcc -c cbin.c

clean:
	rm cshell cshell.o cbin cbin.o
