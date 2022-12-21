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




struct coordinates coor;
bool empty = true;
int nave_id;
void gennave()
{
    coor.x = rand() % (int)(SO_LATO + 1);
    coor.y = rand() % (int)(SO_LATO + 1);
    printf("Creata nave n. %d in posizione %f, %f\n", nave_id, coor.x, coor.y);
}
int main (int argc, char * argv[]){
    
    int mem_id; 
    int sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    nave_id = atoi(argv[1]);
    srand(getpid());
    /* Ottengo l'accesso a IPC obj */
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    /*TEST ERROR*/
    gennave();
    sops.sem_num=1;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    sops.sem_num=2;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    shmdt ( sh_mem );
    exit(0);
}