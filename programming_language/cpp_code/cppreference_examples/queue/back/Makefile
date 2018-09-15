CC=g++
#CFLAGS=-I. --std=c++17 -O0 --no-pie -fPIC
CFLAGS=-I. --std=c++11 -O0 --no-pie -fPIC

TARGET := $(shell ls *.cpp | cut -d '.' -f 1)
$(info "build target: $(TARGET)")

OBJ=$(TARGET).o

$(TARGET).out: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET).out *.o *~ *.out
