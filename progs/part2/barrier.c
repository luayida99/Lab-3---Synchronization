#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "barrier.h"

int nproc = 0;
int count = 0;
int shmid;
Barrier *barrier;

void init_barrier(int numproc) {
    nproc = numproc;
    count = 0;
    shmid = shmget(IPC_PRIVATE, sizeof(Barrier), IPC_CREAT | 0600);
    barrier = (Barrier *) shmat(shmid, NULL, 0);
    //sems[0] is barrier, sems[1] is mutex for count
    sem_init(&barrier->sems[0], 1, 0);
    sem_init(&barrier->sems[1], 1, 1);
}

void reach_barrier() {
    sem_wait(&barrier->sems[1]);
    count = count + 1;
    sem_post(&barrier->sems[1]);
    if (count == nproc) {
        sem_post(&barrier->sems[0]);
    } 
    sem_wait(&barrier->sems[0]);
    sem_post(&barrier->sems[0]);
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        sem_destroy(&barrier->sems[0]);
        sem_destroy(&barrier->sems[1]);
        shmctl(shmid, IPC_RMID, 0);
    }
}


