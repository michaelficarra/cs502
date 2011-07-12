#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "mailbox.h"

#define __NR_mailbox_send   341
#define __NR_mailbox_rcv    342
#define __NR_mailbox_manage 343

int SendMsg(pid_t dest, void *msg, int len, bool block) {
	return syscall(__NR_mailbox_send, dest, msg, len, block);
}

int RcvMsg(pid_t *sender, void *msg, int *len, bool block) {
	return syscall(__NR_mailbox_rcv, sender, msg, len, block);
}

int ManageMailbox(bool stop, int *count) {
	return syscall(__NR_mailbox_manage, stop, count);
}
