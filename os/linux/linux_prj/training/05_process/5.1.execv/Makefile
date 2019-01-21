CC=gcc
#CFLAGS=-I. --std=c++17 -O0 --no-pie -fPIC
CFLAGS=-I. -O0 --no-pie -fPIC

TARGET := $(shell ls *.c | cut -d '.' -f 1)
$(info "build target: $(TARGET)")

OBJ=$(TARGET).o

$(TARGET).out: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET).out *.o *~ *.out
