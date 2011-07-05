/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include "external.c"

int main(int argc, const char* argv[]) {

	srand(time(NULL));
	//srand(0x01010101);

	if(argc < 4) return 1;
	int averageArrivalTime = atoi(argv[1]);
	if(averageArrivalTime < 0) return 1;
	int averageStayTime    = atoi(argv[2]);
	if(averageStayTime < 0) return 1;
	int averageLoopCount   = atoi(argv[3]);
	if(averageLoopCount < 1) return 1;

	int numberOfThreads = randRange(128, 256);
	pthread_t threads[256];

	Gender bathroomType = MALE;

	sem_t bathroomOccupants;
	sem_init(&bathroomOccupants, 0, 0);

	sem_t doorLock;
	sem_init(&doorLock, 0, 1);

	sem_t printLock;
	sem_init(&printLock, 0, 1);

	ThreadArgs **args;
	args = (ThreadArgs**) malloc(numberOfThreads * sizeof(ThreadArgs*));
	if(args == NULL) {
		printf("not able to allocate enough memory\n");
		return 2;
	}
	int t;
	for(t = numberOfThreads - 1; t >= 0; --t) {
		args[t] = (ThreadArgs*) malloc(sizeof(ThreadArgs));
		if(args[t] == NULL) {
			printf("not able to allocate enough memory\n");
			return 2;
		}
		args[t]->gender = randRange(0, 1) ? MALE : FEMALE;
		args[t]->averageArrivalTime = averageArrivalTime;
		args[t]->averageStayTime    = averageStayTime;
		args[t]->averageLoopCount   = averageLoopCount;
		args[t]->bathroomOccupants  = &bathroomOccupants;
		args[t]->bathroomType       = &bathroomType;
		args[t]->doorLock           = &doorLock;
		args[t]->printLock          = &printLock;
		args[t]->id                 = &threads[t];
		pthread_create(&threads[t], NULL, Individual, args[t]);
	}

	for(t = numberOfThreads - 1; t >= 0; --t) {
		pthread_join(threads[t], NULL);
		free(args[t]);
	}
	free(args);

	printf("=====\n");
	printf("executed %d user threads\n", numberOfThreads);

	return 0;
}
