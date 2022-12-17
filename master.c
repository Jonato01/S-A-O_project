#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "shm.h"
#include <stdbool.h>
#include <sys/wait.h>


#define _GNU_SOURCE
#define SO_NAVI 20
#define SO_PORTI 5
#define SO_GIORNI 30
#define SO_MERCI 5
#define SO_SIZE 100 
#define SO_MIN_VITA 2 
#define S0_MAX_VITA 10
#define SO_LATO 100.00    //Dimensioni della mappa in km

 struct merce{
    int id;
    char* nome;
    int lotti;
    int size;
    int vita;
};

void resetSems(int sem_id){
    for(int i = 0; i < NUM_SEMS; i++){
        semctl(sem_id, i, SETVAL, 1);
    }
}

int main(int args,char* argv[]){
    int mem_id, sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    double map[SO_SIZE][SO_SIZE];
    sem_id = semget(getpid()+1,NUM_SEMS,0600 | IPC_CREAT);
    resetSems(sem_id);
    mem_id = shmget (getpid(), sizeof(*sh_mem), 0600 | IPC_CREAT );
    sh_mem = shmat(mem_id, NULL, 0);    
    printf("Creating shm with id: %d\nCreating sem with id:%d\n", mem_id, sem_id);

    LOCK
    sh_mem->cur_idx=0;
    sh_mem->map_size = SO_SIZE;
    printf("MASTER: la mappa è grande %lf\n", sh_mem->map_size);
    sh_mem->j= 100;
    UNLOCK

    for(int i = 0; i < SO_NAVI; i++){
        if(!fork()){
            char c[2];
            sprintf(c, "%d", i);
            char * v[] = {"./nave", c, NULL};
            execve(v[0],v,NULL);
        }
    }

    while(wait(NULL) != -1);
    shmdt ( sh_mem );
    printf("Deleting smh with id %d\n", mem_id);
    shmctl(mem_id , IPC_RMID , NULL);
    printf("Deleting sem with id %d\n", sem_id);
    semctl(sem_id, 0, IPC_RMID);
}
