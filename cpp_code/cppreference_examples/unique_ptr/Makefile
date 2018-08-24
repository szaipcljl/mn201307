CC=g++
CFLAGS=-I. --std=c++17 -O0 --no-pie -fPIC
#CC_VER_GTE48 := $(shell expr `$(CC) -dumpversion | cut -f1-2 -d.` \>= 4.8)

TARGET=unique_ptr

OBJ=$(TARGET).o

$(TARGET).out: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

.PHONY: clean

clean:
	rm -f $(TARGET).out *.o *~ *.out
