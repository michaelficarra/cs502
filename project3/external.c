/* Michael Ficarra (k@wpi.edu) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "external.h"

int randRange(int min, int max) {
    if(min == max) return min;
    if(min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }

    int range = max - min + 1;
    int randMax = RAND_MAX - ((RAND_MAX - range + 1) % range);

    int rnd;
    do {
        rnd = rand();
    } while(rnd > randMax);

    return (rnd % range) + min;
}

void printOccupants(ThreadArgs *args) {
	int occupants;
	sem_getvalue(args->bathroomOccupants, &occupants);
	printf(
		"bathroom contains %d %s%s\n",
		occupants,
		*args->bathroomType == MALE ? "male" : "female",
		occupants == 1 ? "" : "s"
	);
}

void Enter(ThreadArgs *args, Stats *stats) {
	clock_t arrivalTime = clock(), entranceTime;
	do {
		sem_wait(args->doorLock);
		int occupants;
		sem_getvalue(args->bathroomOccupants, &occupants);
		if(occupants == 0 || *args->bathroomType == args->gender) {
			entranceTime = clock();
			stats->waitTime = entranceTime - arrivalTime;
			stats->entranceTime = entranceTime;
			*args->bathroomType = args->gender;
			sem_post(args->bathroomOccupants);
			//printOccupants(args);
			sem_post(args->doorLock);
			break;
		}
		sem_post(args->doorLock);
		continue;
	} while(1);

}
void Leave(ThreadArgs *args, Stats *stats){
	sem_wait(args->doorLock);
	stats->exitTime = clock();
	sem_wait(args->bathroomOccupants);
	//printOccupants(args);
	sem_post(args->doorLock);
}

void *Individual(void *arg) {
	ThreadArgs *args = (ThreadArgs *) arg;

	int i, iterations = randRange(1, args->averageLoopCount * 2);
	Stats *stats;
	stats = (Stats *) malloc(iterations * sizeof(Stats));
	for(i = iterations - 1; i >= 0; --i) {
		usleep(randRange(0, args->averageArrivalTime * 2) * TIME_UNITS);
		Enter(args, &stats[i]);
		usleep(randRange(0, args->averageStayTime * 2) * TIME_UNITS);
		Leave(args, &stats[i]);
	}

	long minWaitTime, maxWaitTime, avgWaitTime, totalWaitTime = 0;
	minWaitTime = maxWaitTime = stats[0].waitTime;
	for(i = iterations - 1; i >= 0; --i) {
		minWaitTime = min(minWaitTime, stats[i].waitTime);
		maxWaitTime = max(maxWaitTime, stats[i].waitTime);
		totalWaitTime += stats[i].waitTime;
	}
	avgWaitTime = round(totalWaitTime / (double) iterations);

	sem_wait(args->printLock);
	printf("=====\n");
	printf(
		"%s thread (%lu) used the bathroom %d time%s\n",
		args->gender == MALE ? "male" : "female",
		*args->id,
		iterations,
		iterations == 1 ? "" : "s"
	);
	printf("total wait time   : %9ld time units\n", clocksToTUs(totalWaitTime));
	printf("average wait time : %9ld time units\n", clocksToTUs(avgWaitTime));
	printf("min wait time     : %9ld time units\n", clocksToTUs(minWaitTime));
	printf("max wait time     : %9ld time units\n", clocksToTUs(maxWaitTime));
	sem_post(args->printLock);

	free(stats);

	return 0;
}
