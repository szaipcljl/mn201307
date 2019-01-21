TARGET=pcihacker_app

$(TARGET): pcihacker_app.o
	gcc -o $(TARGET) pcihacker_app.o

pcihacker_app.o: pcihacker_app.c
	gcc -c pcihacker_app.c

clean:
	rm $(TARGET) $(TARGET).o
