/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <string.h>
#include "mailbox.c"

int main(int argc, char* argv[]) {
	int count = 22, myPID = getpid(), err;
	char msg[MAX_MSG_SIZE];
	int sender, length;

	if((err = ManageMailbox(false, &count))) {
		printf("ManageMailbox failure: %d\n", err);
		return 1;
	};
	if(count) {
		printf("count expected to be zero but instead found %d\n", count);
		return 2;
	}

	if((err = SendMsg(myPID, "test", 5, BLOCK))) {
		printf("SendMsg failure: %d\n", err);
		return 3;
	}

	if((err = ManageMailbox(false, &count))) {
		printf("ManageMailbox failure: %d\n", err);
		return 4;
	};
	if(!count) return 5;

	if((err = RcvMsg(&sender, &msg, &length, BLOCK))) {
		printf("RcvMsg failure: %d\n", err);
		return 6;
	}
	if(sender != myPID) return 7;
	if(strcmp(msg, "test")) return 8;
	if(length != 5) return 9;

	if((err = ManageMailbox(true, &count))) {
		printf("ManageMailbox failure: %d\n", err);
		return 10;
	};
	if(count) return 11;

	return 0;

}
