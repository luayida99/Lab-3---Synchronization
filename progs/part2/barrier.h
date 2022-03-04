#include <semaphore.h>

typedef struct {
    sem_t sems[2];
} Barrier;

void init_barrier(int);
void reach_barrier();
void destroy_barrier(int);
