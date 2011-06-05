# Michael Ficarra (k@wpi.edu)
all: build test

build: doit # shell shell2
doit: doit.c
	gcc -o ./doit ./doit.c
shell: shell.c
	gcc -o ./shell shell.c
shell2: shell2.c
	gcc -o ./shell2 shell2.c

test: test-doit
test-doit: doit
	./doit ls -bhlpq --color=auto
test-shell: doit shell
	./shell < test-shell
test-shell2: doit shell2
	./shell2 < test-shell

clean:
	rm -f {doit,shell,shell2}{,.o}
