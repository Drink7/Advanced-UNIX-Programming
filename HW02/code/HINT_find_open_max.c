#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(void) {
	
	long max = sysconf(_SC_OPEN_MAX);
	printf("%ld\n",max);
	return 0;
}
