# Michael Ficarra (k@wpi.edu)

all: user

user: mailbox.o user.c
	gcc -pthread -Wall -o user user.c

mailbox.o: mailbox.c
	gcc -Wall -o mailbox.o -c mailbox.c

clean:
	rm -f user *.o
