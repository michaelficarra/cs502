/* Michael Ficarra (k@wpi.edu) */

// 20ms time unit
#define TIME_UNITS 20000

typedef enum { MALE, FEMALE } Gender;

typedef struct {
	Gender gender;
	int averageArrivalTime;
	int averageStayTime;
	int averageLoopCount;
	Gender *bathroomType;
	sem_t *bathroomOccupants;
	sem_t *doorLock;
	sem_t *printLock;
	pthread_t *id;
} ThreadArgs;

typedef struct {
	clock_t waitTime;
	clock_t entranceTime;
	clock_t exitTime;
} Stats;

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))
#define round(x) ((long) ((x) >= 0 ? (x) + 0.5 : (x) - 0.5))
#define clocksToTUs(x) round((x) / (CLOCKS_PER_SEC / (double) TIME_UNITS))

extern void Enter(ThreadArgs* a, Stats* s);
extern void Leave(ThreadArgs* a, Stats* s);
extern void *Individual(void *args);

extern void Initialize(Gender g, int a, int s, int l);
