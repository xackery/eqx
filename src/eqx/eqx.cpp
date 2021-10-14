#include <string.h>
#include "stdio.h"
#include "eqx.h"

int main(int argc, char **argv) {
	
	int i = 1;
	bool ignore_collide_tex = true;
	if (argc > 2) {
		if (strcmp(argv[1], "--IncludeCollideTex") == 0) {
			ignore_collide_tex = false;
			i = 2;
		}
	}

	for(; i < argc; ++i) {		
		if (parse(argv[i])) return 0;		
	}
	return 0;
}


bool parse(char* path) {
	printf("parsing %s\n", path);
	return true;
}