#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	char *newpath = "/sys/block/loop0/alignment_offset";
	char* last_occurrence = strrchr(newpath, '/');
	printf("last_occurrence=%s\n", last_occurrence);

	char* first_occurrence = strchr(newpath, '/');
	printf("first_occurrence=%s\n", first_occurrence);


	char *oldpath = "/sys/block/loop0/";
	newpath = oldpath;
	last_occurrence = strrchr(newpath, '/');
	printf("last_occurrence=%s\n", last_occurrence);

	first_occurrence = strchr(newpath, '/');
	printf("first_occurrence=%s\n", first_occurrence);

	return 0;
}

// note:
// strchr, strrchr, strchrnul - locate character in string
// The strrchr() function returns a pointer to the last occurrence of the character c in the string s.
