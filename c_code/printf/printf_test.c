#include <stdio.h>

#define GRAY "30"
#define RED "31"
#define GREEN "32"
#define YELLOW "33"

#define COLOR_STRING(color,content) "\x1b["color";1m"content"\x1b[0m"

int main(int argc, const char *argv[])
{

	size_t offset = 0xffff880000000000;
    printf("\x1b[33;1m[+]\x1b[0m Setting physical offset to 0x%zx\n", offset);
	printf(COLOR_STRING(RED,"[!]")" Program requires root privileges (or read access to /proc/<pid>/pagemap)!\n");

	printf(COLOR_STRING(GRAY,"gg")"\n");
	printf(COLOR_STRING(RED,"gg")"\n");
	printf(COLOR_STRING(GREEN,"gg")"\n");
	printf(COLOR_STRING(YELLOW,"gg")"\n");

	size_t correct = 0, wrong = 0;
	int progress = 0;

	//[] 数组运算符
	printf("%c\n", "/-\\|"[2]);

	while (1) {
		correct ++;

		if ((correct + wrong)%3) {
			wrong++;
		}
#if 0
		printf("\r\x1b[34;1m[%c]\x1b[0m Success rate: %.2f%% (read %zd values)    ",
				"/-\\|"[(progress++ / 100) % 4],
				(100.f * (double)correct) / (double)(correct + wrong),
				correct + wrong);
#else
		//use \r, don't use \n in the end
		//printf("\r"COLOR_STRING(RED,"gg"));
		printf("\r"COLOR_STRING(RED,"%c"), "abcd"[(progress++ / 100) % 4]);
#endif

	}

	return 0;
}
