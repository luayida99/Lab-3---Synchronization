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
Barrier *barrier;

void init_barrier(int numproc) {
    //shared memory for nproc and count
    shmid_c = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if (shmid_c == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    }

    count = (int*) shmat(shmid_c, NULL, 0);
    if (count == (int*) -1) {
        printf("Cannot attach to shared memory!\n");
        exit(1);
    }

    shmid_n = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0600);
    if (shmid_n == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    }

    nproc = (int*) shmat(shmid_n, NULL, 0);
    if (nproc == (int*) -1) {
        printf("Cannot attach to shared memory!\n");
        exit(1);
    }

    *nproc = numproc;
    *count = 0;
    
    printf("Count: %d", *count);
    //shared memory for barrier
    shmid_b = shmget(IPC_PRIVATE, sizeof(Barrier), IPC_CREAT | 0600);
    barrier = (Barrier *) shmat(shmid_b, NULL, 0);

    //sems[0] is barrier, sems[1] is mutex for count
    sem_init(&barrier->sems[0], 1, 0);
    sem_init(&barrier->sems[1], 1, 1);
}

void reach_barrier() {
    printf("Reached barrier");
    sem_wait(&barrier->sems[1]);
    *count++;
    printf("%d", *count);
    sem_post(&barrier->sems[1]);
    if (*count == *nproc) {
        sem_post(&barrier->sems[0]);
    } 
    sem_wait(&barrier->sems[0]);
    sem_post(&barrier->sems[0]);
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        sem_destroy(&barrier->sems[0]);
        sem_destroy(&barrier->sems[1]);
        shmctl(shmid_c, IPC_RMID, 0);
        shmctl(shmid_b, IPC_RMID, 0);
        shmctl(shmid_n, IPC_RMID, 0);
    }
}


