#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_PROCESSES 5

int main() {

    int i, j, pid, shmid;

    shmid = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(sem_t), IPC_CREAT | 0600);
    sem_t* sems = (sem_t*) shmat(shmid, NULL, 0);

    for (int k = 0; k < NUM_PROCESSES; k++) {
        sem_init(&sems[k], 1, 0);
    }
    
    for(i=0; i<NUM_PROCESSES; i++)
    {
        if((pid = fork()) == 0) {
            break;
        }
    }

    if(pid == 0) {
        sem_wait(&sems[i]);
        printf("I am child %d\n", i);

        for(j = i*10; j<i*10 + 10; j++){
            printf("%d ", j);
            fflush(stdout);
            usleep(250000);
        }

        printf("\n\n");
        
        if (i + 1 != NUM_PROCESSES) {
          sem_post(&sems[i+1]);
        }
        sem_destroy(&sems[i]);
    }
    else {
        sem_post(&sems[0]);
        for(i=0; i<NUM_PROCESSES; i++) 
            wait(NULL);
    }

    shmctl(shmid, IPC_RMID, 0);

}

