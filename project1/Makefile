# Michael Ficarra (k@wpi.edu)
all: test-shell2

doit: doit.c
	gcc -o ./doit ./doit.c
shell: shell.c
	gcc -o ./shell shell.c
shell2: shell2.c
	gcc -o ./shell2 shell2.c

test-doit: doit
	./doit ls -bhlpq --color=auto
test-shell: doit shell
	./shell < test-shell
test-shell2: doit shell2
	./shell2 < test-shell2

clean:
	rm -f {doit,shell,shell2}{,.o}
