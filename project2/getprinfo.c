/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include "prinfo.h"

int main() {
	struct prinfo info;
	int pid, children = 4, i;

	printf("getprinfo(null) returns %ld\n", getprinfo(NULL));
	printf("getprinfo(&info) returns %ld\n", getprinfo(&info));

	if(!(pid = fork())) {
		for(i = 0; i < children; i++) {
			if(fork()) continue;
			sleep(2);
			return 0;
		}
	}

	getprinfo(&info);

	if((pid && fork()) || !fork())
		printf(
			"Contents of struct prinfo:\n"
			"  { state           : %ld\n"
			"  , pid             : %d\n"
			"  , parent_pid      : %d\n"
			"  , youngest_child  : %d\n"
			"  , younger_sibling : %d\n"
			"  , older_sibling   : %d\n"
			"  , uid             : %u\n"
			"  , comm            : %s\n"
			"  , start_time      : %lus after boot\n"
			"  , user_time       : %lums\n"
			"  , sys_time        : %lums\n"
			"  , cutime          : %lums\n"
			"  , cstime          : %lums\n"
			"  }\n",
			info.state,
			info.pid,
			info.parent_pid,
			info.youngest_child,
			info.younger_sibling,
			info.older_sibling,
			info.uid,
			info.comm,
			info.start_time,
			info.user_time,
			info.sys_time,
			info.cutime,
			info.cstime
		);

	return 0;
}
