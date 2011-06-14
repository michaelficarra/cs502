/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define true  1
#define false 0
#define yes   1
#define no    0
typedef unsigned int bool;

#define DEBUG 0
#define MAX_LINE 4096
#define MAX_ARG  256

int main(int argc, char* argv[]) {

	int i, l, a, pid, throwaway;
	char c, quoteType, cwd[4096], inputStr[MAX_LINE + 1];
	char* args[33];
	bool inQuotes, escaped, piped = !isatty(fileno(stdin));

	getcwd(cwd, 4096);

	do {
		printf("==> ");

		inputStr[0] = '\0';
		inQuotes = escaped = no;
		while((c = getchar())) {
			if(piped && ((c >= ' ' && c <= '~') || c == '\n' || c == '\r')) putchar(c);
			switch(c) {
				case EOF :
					if(strcmp(inputStr, "") != 0) continue;
					printf("\n");
					goto breakRepl;
				case '"' :
				case '\'':
					if(!escaped) {
						if(!inQuotes) {
							inQuotes = yes;
							quoteType = c;
						} else {
							if(quoteType == c) inQuotes = no;
						}
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
		memcpy(args[0], cwd, strlen(cwd));
		strcat(args[0], "/doit");

		args[1] = malloc(sizeof(char) * (MAX_ARG + 1));
		args[1][0] = '\0';

		inQuotes = escaped = no;
		l = strlen(inputStr);
		a = 1;
		for(i = 0; i < l; i++) {
			switch(c = inputStr[i]) {
				case '"' :
				case '\'':
					if(!escaped) {
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
					}
					break;
				case ' ' :
				case '\t':
					if(inQuotes || escaped) strncat(args[a], &c, 1);
					else if(strcmp(args[a], "") == 0) break;
					else {
						args[++a] = malloc(sizeof(char) * (MAX_ARG + 1));
						args[a][0] = '\0';
					}
					break;
				case '\r':
				case '\n':
					if(inQuotes) strncat(args[a], &c, 1);
					else if(strcmp(args[a], "") == 0) break;
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

		if(strcmp(args[1], "") == 0) continue;

		if(strcmp(args[a], "") != 0) ++a;
		args[a] = NULL;

		if(strcmp(args[1], "exit") == 0)
			goto breakRepl;
		if(strcmp(args[1], "cd") == 0) {
			chdir(args[2]);
			continue;
		}


		if(DEBUG) {
			printf("%s", args[0]);
			a = 0;
			while(args[++a])
				printf(" [%s]", args[a]);
		}


		pid = fork();
		if(pid < 0) {
			printf("Forking error occured.\n");
			return 1;
		} else if(pid) {
			waitpid(pid, &throwaway, 0);
		} else {
			execvp(args[0], args);
			printf("Execution failed with error code %d.\n", errno);
			return 1;
		}

	} while(true);
	breakRepl:

	return 0;
}
