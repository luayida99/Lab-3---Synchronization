#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>

int nproc = 0;
int count = 0;
int shmid;
sem_t *barrier;

void init_barrier(int numproc) {
    nproc = numproc;
    count = 0;
    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    barrier = (sem_t *) shmat(shmid, NULL, 0);
    sem_init(barrier, 1, 0);
}

void reach_barrier() {
    count = count + 1;
    if (count == nproc) {
        sem_post(barrier);
    } else {
        sem_wait(barrier);
        sem_post(barrier);
    }
}

void destroy_barrier(int my_pid) {
    if(my_pid != 0) {
        sem_destroy(barrier);
        shmctl(shmid, IPC_RMID, 0);
    }
}


