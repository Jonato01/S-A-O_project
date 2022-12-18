#define _GNU_SOURCE
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

#define SO_BANCHINE 4

struct coordinates coor;
int porto_id;

void creaPorto(){
    //gestire caso in cui si provi a creare più porti nelle stesse coordinate
}

int main(int argc, char * argv[]){
    srand(getpid());
    porto_id = atoi(argv[1]);

    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 | IPC_CREAT);
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600 | IPC_CREAT);
    sh_mem = shmat(mem_id, NULL, 0);
    //TEST ERROR

    switch(porto_id){
        case 0:
            coor.x = 0;
            coor.y = 0;
            break;
        case 1:
            coor.x = 0;
            coor.y = SO_LATO;
            break;
        case 2:
            coor.x = SO_LATO;
            coor.y = 0;
            break;
        case 3:
            coor.x = SO_LATO;
            coor.y = SO_LATO;
            break;
        default:
            coor.x = rand() % (int)(SO_LATO + 1);
            coor.y = rand() % (int)(SO_LATO + 1);
            creaPorto();
            break;
    }
    LOCK
    //sh_mem->all_ports[porto_id].x = coor.x;
    //sh_mem->all_ports[porto_id].y = coor.y;
    UNLOCK

    printf("Creato il porto %d in posizione %lf, %lf\n", porto_id, coor.x, coor.y);
    exit(0);
}