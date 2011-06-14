# Michael Ficarra (k@wpi.edu)

all: doit shell shell2

doit: doit.c
	gcc -Wall -o ./doit ./doit.c
shell: shell.c
	gcc -Wall -o ./shell shell.c
shell2: shell2.c
	gcc -Wall -o ./shell2 shell2.c

clean:
	rm -f {doit,shell,shell2}{,.o}
