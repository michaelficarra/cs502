/* Michael Ficarra (k@wpi.edu) */

#include <stdbool.h>
#include <sys/types.h>

#define NO_BLOCK false
#define BLOCK    true

#define MAX_MSG_SIZE 128

#define MAILBOX_FULL    1001
#define MAILBOX_EMPTY   1002
#define MAILBOX_STOPPED 1003
#define MAILBOX_INVALID 1004
#define MSG_TOO_LONG    1005
#define MSG_ARG_ERROR   1006
#define MAILBOX_ERROR   1007

int SendMsg(pid_t dest, void *msg, int len, bool block);
int RcvMsg(pid_t *sender, void *msg, int *len, bool block);
int ManageMailbox(bool stop, int *count);
