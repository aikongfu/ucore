#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void die(const char *str, ...) {
		va_list args;
		va_start(args, str);
		vfprintf(stderr, str, args);
		va_end(args);
		fputc('\n', stderr);
		exit(1);

}

int main(int argc, char *argv[]) {
		
		int fd;
		ssize_t size;
		char buf[512];

		if (2 != argc) {
				
		}

}
