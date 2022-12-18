#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "shm.h"

#define _GNU_SOURCE
#define SO_SPEED 30
#define SO_CAPACITY 300

struct coordinates coor;
bool empty = true;
int nave_id;

int main (int argc, char * argv[]){
    nave_id = atoi(argv[1]);
    srand(getpid());
    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 | IPC_CREAT);
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600 | IPC_CREAT);
    sh_mem = shmat(mem_id, NULL, 0);
    //TEST ERROR

    coor.x = rand() % (int)(SO_LATO + 1);
    coor.y = rand() % (int)(SO_LATO + 1);
    printf("Creata nave n. %d in posizione %lf, %lf\n", nave_id, coor.x, coor.y);
    exit(0);
}