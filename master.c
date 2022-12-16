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

int main(int args,char* argv[]){
    int mem_id, sem_id;
    int status; //da togliere
    struct shared_data * sh_mem;
    struct sembuf sops;
    double map[SO_SIZE][SO_SIZE];
    sem_id = semget(getpid()+1,3,0660 | IPC_CREAT);
    mem_id = shmget (getpid(), sizeof(*sh_mem), 0600 | IPC_CREAT );
    sh_mem = shmat(mem_id, NULL, 0);
    char * v[]={"./nave","ciao",NULL};
    if(!fork())
    execve(v[0],v,NULL);
    else{
    sleep(2);
    double c;
    int i=0;
  
      
    sh_mem->cur_idx=0;
    sh_mem->map_size = SO_SIZE;
    sh_mem->j= 100;
 printf("%lf",sh_mem->map_size); 
    shmdt ( sh_mem );
    shmctl ( mem_id , IPC_RMID , NULL );
}}
