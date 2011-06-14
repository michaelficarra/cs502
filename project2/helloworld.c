/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#define __NR_helloworld 341

long helloworld() {
	return (long) syscall(__NR_helloworld);
}

int main() {
	printf(
		"The return code from the helloworld system call is %ld\n",
		helloworld()
	);
	return 0;
}
