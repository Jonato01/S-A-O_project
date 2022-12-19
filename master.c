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
#define SO_GIORNI 30
#define NAVI_PATH_NAME "./nave"
#define PORTI_PATH_NAME "./porto"
#define SO_MERCI 5
#define SO_SIZE 100 
#define SO_MIN_VITA 2 
#define S0_MAX_VITA 10
/*Dimensioni della mappa in km*/
void resetSems(int sem_id);
void genmerci(struct merce *merc);
void genporti();
void gennavi();
void resetSems(int sem_id){
    int i;
    for(i = 0; i < NUM_SEMS; i++){
        semctl(sem_id, i, SETVAL, 1);
    }
}
void gennavi()
{
    int i;
    char *c;
    char * argsnavi[]={NAVI_PATH_NAME,NULL,NULL};
    c=calloc(1,sizeof(int));
    
    /*creazione navi*/
    for(i = 0; i < SO_NAVI; i++){
        if(!fork()){
            sprintf(c, "%d", i);
            argsnavi[1]=c;
            execve(NAVI_PATH_NAME,argsnavi,NULL);
            perror("Execve navi er");
	    	exit(1);
        }
    }
}
void genporti()
{
    int i;
    char *c;
    char * argsnavi[]={PORTI_PATH_NAME,NULL,NULL};
    c=calloc(1,sizeof(int));
    
    /*creazione porti*/
    for(i = 0; i < SO_PORTI; i++){
        if(!fork()){
            sprintf(c, "%d", i);
            argsnavi[1]=c;
            execve(PORTI_PATH_NAME,argsnavi,NULL);
            perror("Execve porti er");
	    	exit(1);
        }
    }
}
void genmerci(struct merce *merc)
{
    
}
int main(int args,char* argv[]){
    int mem_id;
    int sem_id; 
    struct shared_data * sh_mem;
    struct sembuf sops;
    
    struct merce *merc;
    /*creazione IPC obj*/
    sem_id = semget(getpid()+(pid_t)1,NUM_SEMS,0600 | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 1);
    resetSems(sem_id);
    mem_id = shmget (getpid(), sizeof(*sh_mem), 0600 | IPC_CREAT );
    sh_mem = shmat(mem_id, NULL, 0);    
    printf("Creating shm with id: %d\nCreating sem with id:%d\n", mem_id, sem_id);

    LOCK
        sh_mem->all_ports = calloc(SO_PORTI, sizeof(*(sh_mem->all_ports)));
    UNLOCK
    gennavi();
    genporti();
    
    shmdt ( sh_mem );
    printf("Deleting smh with id %d\n", mem_id);
    shmctl(mem_id , IPC_RMID , NULL);
    printf("Deleting sem with id %d\n", sem_id);
    semctl(sem_id, 0, IPC_RMID);
    return 0;
}