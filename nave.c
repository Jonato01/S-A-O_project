#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <bool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "smh.h"

#define _GNU_SOURCE
#define SO_SPEED 30
#define SO_CAPACITY 300

double coordinates[2];
bool empty = true;

int main (){
    srand(getpid());
    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    double map_size;
    mem_id = (getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    //TEST ERROR
    LOCK
    double map_size = sh_mem->map_size;
    UNLOCK
    printf("%lf", map_size);
}