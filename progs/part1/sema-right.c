//
// Program to create shared semaphores
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>

int main() {
    int shmid, pid;
    sem_t *sem;

    shmid = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0600);
    if (shmid == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    }
    sem = (sem_t *) shmat(shmid, NULL, 0);
    if (sem == (sem_t*) -1) {
        printf("Cannot attach to shared memory!\n");
        exit(1);
    }

    sem_init(sem, 1, 0);

    if((pid = fork()) != 0) {
        printf("Parent!. Making my child wait for 1 second.\n");
        sleep(1);
        sem_post(sem);
        wait(NULL);
        sem_destroy(sem);
        shmctl(shmid, IPC_RMID, 0);
    }
    else
    {
        sem_wait(sem);
        printf("Child! Waited 1 second for parent.\n");
    }
}

