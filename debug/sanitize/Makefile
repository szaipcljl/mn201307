all:
	gcc -ggdb -o mem_leak mem_leak.c -fsanitize=address -fno-omit-frame-pointer
	g++ -ggdb -o heap-use-after-free heap-use-after-free.cc -fsanitize=address -fno-omit-frame-pointer
	g++ -ggdb -o heap-buffer-overflow heap-buffer-overflow.cc -fsanitize=address -fno-omit-frame-pointer
	g++ -ggdb -o stack-buffer-overflow stack-buffer-overflow.cc -fsanitize=address -fno-omit-frame-pointer
	g++ -ggdb -o global-buffer-overflow global-buffer-overflow.cc -fsanitize=address -fno-omit-frame-pointer
clean:
	rm mem_leak heap-use-after-free heap-buffer-overflow stack-buffer-overflow global-buffer-overflow
