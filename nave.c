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

int main (){
    int datogliere;
    srand(getpid());
    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    
    double map_size;
    sem_id=semget(getpid()+1,sizeof(sops),0660 | IPC_CREAT);
    mem_id = shmget(getppid(), 3, 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    //TEST ERROR
    LOCK
    double map__size = sh_mem->map_size;
    UNLOCK
   
    shmdt ( sh_mem );
    shmctl ( mem_id , IPC_RMID , NULL );
     exit(map__size);
}