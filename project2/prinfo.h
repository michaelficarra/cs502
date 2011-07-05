/* Michael Ficarra (k@wpi.edu) */

#ifndef _LINUX_PRINFO_H
#define _LINUX_PRINFO_H

#include <unistd.h>
#include <sys/types.h>
//#include <sys/syscall.h>

#define __NR_getprinfo 342

struct prinfo {
	long state;			// current state of process
	pid_t pid;			// process ID of this process
	pid_t parent_pid;		// process ID of parent
	pid_t youngest_child;	// process ID of youngest child
	pid_t younger_sibling;	// process ID of next younger sibling
	pid_t older_sibling;		// process ID of next older sibling
	uid_t uid;			// user ID of process owner
	char comm[16];		// name of program executed
	unsigned long start_time;	// process start time in seconds since the Epoch
	unsigned long user_time;	// CPU time spent in user mode (milliseconds)
	unsigned long sys_time;	// CPU time spend in system mode (milliseconds)
	unsigned long cutime;	// total user time of children (milliseconds)
	unsigned long cstime;	// total system time of children (milliseconds)
};

long getprinfo(struct prinfo *info) {
	return (long) syscall(__NR_getprinfo, info);
}

#endif
