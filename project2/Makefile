# Michael Ficarra (k@wpi.edu)

all: helloworld getprinfo

helloworld: helloworld.c
	gcc -Wall -o helloworld helloworld.c

getprinfo: getprinfo.c
	gcc -Wall -o getprinfo getprinfo.c

clean:
	rm -f {getprinfo,helloworld}{,.o}
