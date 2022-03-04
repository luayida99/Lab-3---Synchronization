#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "barrier.h"

int* nproc;
int* count;
int* shm;
int shmid_c;
int shmid_n;
int shmid_b;
sem_t* sems;

void init_barrier(int numproc) {
    //shared memory for nproc and count
    shmid_c = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    count = (int*) shmat(shmid_c, NULL, 0);

    shmid_n = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    nproc = (int*) shmat(shmid_n, NULL, 0);

    *nproc = numproc;
    *count = 0;

    //shared memory for barrier
    shmid_b = shmget(IPC_PRIVATE, 2 * sizeof(sem_t), IPC_CREAT | 0600);
    sems = (sem_t*) shmat(shmid_b, NULL, 0);

    //sems[0] is barrier, sems[1] is mutex for count
    sem_init(&sems[0], 1, 0);
    sem_init(&sems[1], 1, 1);
}

void reach_barrier() {
    sem_wait(&sems[1]);
    *count = *count + 1;
    sem_post(&sems[1]);
    if (*count == *nproc) {
        sem_post(&sems[0]);
    } 
    sem_wait(&sems[0]);
    sem_post(&sems[0]);
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        sem_destroy(&sems[0]);
        sem_destroy(&sems[1]);
        shmctl(shmid_c, IPC_RMID, 0);
        shmctl(shmid_b, IPC_RMID, 0);
        shmctl(shmid_n, IPC_RMID, 0);
    }
}


