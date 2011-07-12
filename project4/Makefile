# Michael Ficarra (k@wpi.edu)

all: user

user: mailbox.c user.c
	gcc -pthread -Wall -o user user.c

clean:
	rm -f {user,mailbox}{,.o}
