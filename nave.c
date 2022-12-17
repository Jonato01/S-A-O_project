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

double coordinates[2];
bool empty = true;

int main (int argc, char * argv[]){
    int nave_id = atoi(argv[1]);
    srand(getpid());
    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    
    sem_id = semget(getppid()+1,sizeof(sops),0600 | IPC_CREAT);
    mem_id = shmget(getppid(), NUM_SEMS, 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    //TEST ERROR

    LOCK
    double map__size = sh_mem->map_size;
    UNLOCK

    coordinates[0] = rand() % (int)map__size + 1;
    coordinates[1] = rand() % (int)map__size + 1;
    printf("Creata nave n. %d in posizione %lf, %lf\n", nave_id, coordinates[0], coordinates[1]);
    exit(0);
}