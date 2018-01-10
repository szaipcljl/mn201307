CC=g++
CFLAGS=-I. --std=c++11 -O0 --no-pie -fPIC
#CC_VER_GTE48 := $(shell expr `$(CC) -dumpversion | cut -f1-2 -d.` \>= 4.8)

OBJ=max_element.o

ifeq ($(CC_VER_GTE48), 1)
#CFLAGS += -mrtm -DHAS_COMPILER_RTM_SUPPORT
endif

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

max_element: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f max_element *.o *~ *.out
