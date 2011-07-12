#include "mailbox.c"

int main(int argc, char* argv[]) {
	int count = 1;
	ManageMailbox(false, &count);
	return count;
}
