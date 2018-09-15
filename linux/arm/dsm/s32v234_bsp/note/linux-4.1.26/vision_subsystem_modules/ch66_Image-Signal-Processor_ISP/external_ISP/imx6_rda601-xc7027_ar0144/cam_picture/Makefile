INSTALLROOT=$(PWD)

CC=arm-linux-gnueabihf-gcc
CPP=arm-linux-gnueabihf-g++
APP_BINARY=cam_picture

OBJECTS= video_picture.o

all:	cam_picture

clean:
	@echo "Cleaning up directory."
	rm -f *.a *.o $(APP_BINARY) core *~

cam_picture:	$(OBJECTS)
	$(CC)	$(CFLAGS) $(OBJECTS) -o $(APP_BINARY)
	chmod 755 $(APP_BINARY)

