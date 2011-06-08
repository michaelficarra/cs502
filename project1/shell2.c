/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#define true  1
#define false 0
#define yes   1
#define no    0
typedef unsigned int bool;

#define DEBUG 0
#define MAX_LINE 4096
#define MAX_ARG  256

typedef struct timeval Time;

typedef struct Process {
	char *cmdStr;
	int  pid;
	int  jobId;
	Time startTime;
} Process;

typedef struct ProcessList {
	Process            *process;
	struct ProcessList *next;
} ProcessList;

bool addJob(ProcessList **head, Process *proc) {
	ProcessList *tail = *head;

	ProcessList *node = (ProcessList*) malloc(sizeof(ProcessList));
	if(node == NULL) return false;
	node->process = proc;
	node->next = NULL;

	if(*head == NULL) {
		*head = node;
	} else {
		while(tail->next != NULL) tail = tail->next;
		tail->next = node;
	}
	return true;
}

bool removeJob(ProcessList **jobs, int pid) {
	ProcessList **cursor = jobs;
	while(*cursor != NULL) {
		if((*cursor)->process->pid == pid) {
			*cursor = (*cursor)->next;
			return true;
		}
		cursor = &(*cursor)->next;
	}
	return false;
}

Process* getJob(ProcessList* jobs, int pid) {
	while(jobs != NULL) {
		if(jobs->process->pid == pid)
			return jobs->process;
		jobs = jobs->next;
	}
	return NULL;
}

void printJobs(ProcessList *jobs) {
	ProcessList *job = jobs;
	Process *proc = NULL;
	while(job != NULL) {
		proc = job->process;
		if(proc == NULL) continue;
		printf("[%d] %d: %s\n", proc->jobId, proc->pid, proc->cmdStr);
		job = job->next;
	}
}


void printStatistics(int pid, Time startTime, Time endTime) {
	struct rusage stats;
	getrusage(RUSAGE_CHILDREN, &stats);

	static long int ru_majflt = 0, ru_minflt = 0, ru_nivcsw = 0, ru_nvcsw = 0;
	long int secs, usecs;
	usecs = endTime.tv_usec - startTime.tv_usec;
	secs  = endTime.tv_sec  - startTime.tv_sec;
	while(usecs < 0) {
		usecs += 1000000;
		secs--;
	}

	printf(" +-----\n");
	printf(
		" | Wall-clock time  : %li seconds, %6li microseconds\n",
		secs,
		usecs
		);
	printf(
		" | CPU time         : %li seconds, %6li microseconds\n",
		stats.ru_stime.tv_sec  + stats.ru_utime.tv_sec,
		stats.ru_stime.tv_usec + stats.ru_utime.tv_usec
		);
	printf(
		" | Context switches : %li involuntary, %li voluntary\n",
		stats.ru_nivcsw - ru_nivcsw,
		stats.ru_nvcsw - ru_nvcsw
		);
	ru_nivcsw += stats.ru_nivcsw - ru_nivcsw;
	ru_nvcsw  += stats.ru_nvcsw  - ru_nvcsw;
	printf(
		" | Page faults      : %li\n",
		stats.ru_majflt - ru_majflt
		);
	ru_majflt += stats.ru_majflt - ru_majflt;
	printf(
		" | Reclaimed pages  : %li\n",
		stats.ru_minflt - ru_minflt
		);
	ru_minflt += stats.ru_minflt - ru_minflt;
	printf(" +-----\n");
}


int main(int argc, char* argv[]) {

	int i, l, a, pid, jobId = 1, status;
	char c, quoteType, cwd[4096], *inputStr, *args[33];
	bool inQuotes, escaped, isBgProc, piped = !isatty(fileno(stdin));
	Process *bgProc = NULL;
	ProcessList *jobs = NULL;
	Time startTime, endTime;

	getcwd(cwd, 4096);

	repl: do {
		printf("==> ");

		inputStr = (char*) malloc(MAX_LINE * sizeof(char));
		inQuotes = escaped = no;
		readInput: while(c = getchar()) {
			if(piped && (c >= ' ' && c <= '~' || c == '\n' || c == '\r')) putchar(c);
			switch(c) {
				case EOF :
					if(strcmp(inputStr, "")) continue;
					printf("\n");
					goto breakRepl;
				case '"' :
				case '\'':
					if(!escaped)
						if(!inQuotes) {
							inQuotes = yes;
							quoteType = c;
						} else {
							if(quoteType == c) inQuotes = no;
						}
					strncat(inputStr, &c, 1);
					break;
				case '\r':
				case '\n':
					if(!inQuotes && !escaped) goto breakReadInput;
					strncat(inputStr, &c, 1);
					break;
				case '\b':
					i = strlen(inputStr) - 1;
					if(i < 0) i = 0;
					c = inputStr[i];
					if((c == '"' || c == '\'') && !(i > 0 && inputStr[i - 1] == '\\' && i > 1 && inputStr[i - 2] != '\\')) {
						if(!inQuotes) {
							inQuotes = yes;
							quoteType = c;
						} else {
							if(quoteType == c) inQuotes = no;
						}
					} else if(c == '\\') {
						escaped = !escaped;
					}
					inputStr[i] = '\0';
					break;
				case '\\':
					escaped = !escaped;
					strncat(inputStr, &c, 1);
					continue;
				default:
					strncat(inputStr, &c, 1);
			}
			escaped = no;
		}
		breakReadInput:

		if(DEBUG) printf("%s\n", inputStr);


		args[0] = malloc(sizeof(char) * (MAX_ARG + 1));
		args[0][0] = '\0';

		inQuotes = escaped = no;
		l = strlen(inputStr);
		a = 0;
		parseInput: for(i = 0; i < l; i++) {
			switch(c = inputStr[i]) {
				case '"' :
				case '\'':
					if(!escaped)
						if(!inQuotes) {
							inQuotes = yes;
							quoteType = c;
						} else {
							if(quoteType == c) {
								inQuotes = no;
								args[++a] = malloc(sizeof(char) * (MAX_ARG + 1));
								args[a][0] = '\0';
							}
						}
					break;
				case ' ' :
				case '\t':
					if(inQuotes || escaped) strncat(args[a], &c, 1);
					else if(!strcmp(args[a], "")) break;
					else {
						args[++a] = malloc(sizeof(char) * (MAX_ARG + 1));
						args[a][0] = '\0';
					}
					break;
				case '\r':
				case '\n':
					if(inQuotes) strncat(args[a], &c, 1);
					else if(!strcmp(args[a], "")) break;
					else if(!escaped){
						args[++a] = malloc(sizeof(char) * (MAX_ARG + 1));
						args[a][0] = '\0';
					}
					break;
				case '\\':
					if(escaped) strncat(args[a], "\\", 1);
					escaped = !escaped;
					continue;
				default:
					if(escaped) strncat(args[a], "\\", 1);
					strncat(args[a], &c, 1);
			}
			escaped = no;
		}
		breakParseInput:

		while(pid = waitpid(WAIT_ANY, &status, WNOHANG)) {
			if(pid < 0) break;
			bgProc = getJob(jobs, pid);
			if(bgProc == NULL) return; // just in case
			printf("[%d] %d %s\n", bgProc->jobId, bgProc->pid, bgProc->cmdStr);
			removeJob(&jobs, pid);
			gettimeofday(&endTime, NULL);
			printStatistics(pid, bgProc->startTime, endTime);
		}

		if(!strcmp(args[0], "")) continue;

		if(strcmp(args[a], "")) ++a;
		args[a] = NULL;

		if(!strcmp(args[0], "exit"))
			goto breakRepl;
		if(!strcmp(args[0], "cd")) {
			chdir(args[1]);
			continue;
		}
		if(!strcmp(args[0], "jobs")) {
			printJobs(jobs);
			continue;
		}



		if(DEBUG) {
			printf("%s", args[0]);
			a = 0;
			while(args[++a])
				printf(" [%s]", args[a]);
			printf("\n");
		}


		isBgProc = !strcmp(args[a - 1], "&");
		if(isBgProc) args[--a] = NULL;

		pid = fork();
		if(pid < 0) {
			printf("Forking error %d occured.\n", errno);
			return 1;
		} else if(pid) {
			gettimeofday(&startTime, NULL);
			if(isBgProc) {
				bgProc = (Process*) malloc(sizeof(Process));
				bgProc->cmdStr = inputStr;
				bgProc->pid = pid;
				bgProc->jobId = jobId;
				bgProc->startTime = startTime;
				addJob(&jobs, bgProc);
				printf("[%d] %d\n", jobId, pid);
				++jobId;
			} else {
				waitpid(pid, &status, 0);
				gettimeofday(&endTime, NULL);
				printStatistics(pid, startTime, endTime);
			}
		} else {
			execvp(args[0], args);
			printf("Execution failed with error code %d.\n", errno);
			return 1;
		}

	} while(true);
	breakRepl:

	return 0;
}
