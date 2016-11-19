circ_buf_test: main.o circ_buf.o
	gcc -o circ_buf_test main.o circ_buf.o -lpthread
main.o: main.c
	gcc -c main.c
circ_buf.o: circ_buf.c
	gcc -c circ_buf.c
clean:
	rm circ_buf_test main.o circ_buf.o

