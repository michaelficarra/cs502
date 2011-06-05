/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char* argv[]) {

	int i, pid;
	char cmd[129];
	struct timeval starttime, endtime;

	gettimeofday(&starttime,NULL);

	pid = fork();
	if(pid<0) {
		// fork failed
		printf("Forking error occured.\n");
		return 1;
	} else if(pid) {
		// parent
		wait(pid);
		gettimeofday(&endtime, NULL);

		struct rusage stats;
		getrusage(RUSAGE_CHILDREN,&stats);

		long int secs, usecs;
		usecs = endtime.tv_usec - starttime.tv_usec;
		secs  = endtime.tv_sec  - starttime.tv_sec;
		while(usecs < 0) {
			usecs += 1000000;
			secs--;
		}

		printf("  ---\n");
		printf(
			"  Wall-clock time  : %li seconds, %6li microseconds\n",
			secs,
			usecs
			);
		printf(
			"  CPU time         : %li seconds, %6li microseconds\n",
			stats.ru_stime.tv_sec  + stats.ru_utime.tv_sec,
			stats.ru_stime.tv_usec + stats.ru_utime.tv_usec
			);
		printf(
			"  Context switches : %li involuntary, %li voluntary\n",
			stats.ru_nivcsw,
			stats.ru_nvcsw
			);
		printf(
			"  Page faults      : %li\n",
			stats.ru_majflt
			);
		printf(
			"  Reclaimed pages  : %li\n",
			stats.ru_minflt
			);
	} else {
		// child
		if(argc>1) {
			execvp(argv[1], argv + 1);
			printf("Command execution failed.\n");
		} else {
			printf("No arguments have been provided.\n");
		}
		return 1;
	}

	return 0;
}
