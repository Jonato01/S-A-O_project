#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>
#include "shm.h"

struct coordinates coor;
struct sembuf sops;
struct shared_data * sh_mem;
int sem_id;
bool empty = true;
int nave_id;
double carico;

void ordinaporti(){
    /*Più facile con un vettore di sole coordinate*/
}

/*MODIFICARE IN MODO DA AVERE I PORTI GIÀ ORDINATI PER DISTANZA*/
int getnporto(){
    int x = -1;
    int i;
    int j;
    bool flag = false;
    double d = SO_LATO * sqrt(2);
    LOCK
    if(carico < SO_SIZE){
        for(i = 0; i < SO_PORTI; i++){
            if(DISTANCE(sh_mem->porti[i].coord, coor) < d){
                d = DISTANCE(sh_mem->porti[i].coord, coor);
                if(sh_mem->porti[i].banchinelibere > 0){
                    for(j = 0; j < MERCI_RIC_OFF && sh_mem->porti[i].off[j].size + carico < SO_SIZE; j++){
                        if(!sh_mem->porti[i].off[j].pre){
                            flag = true;
                            sh_mem->porti[i].off[j].pre = true;
                            carico += sh_mem->porti[i].off[j].size;
                            printf("Nave %d: prenotata merce %d dal porto %d\n", nave_id, sh_mem->porti[i].off[j].id, i);
                        }
                    }
                }
                if(flag){
                    x = i;
                    break;
                }
            }
        }
        if(!flag)
            printf("Nave %d: nessun porto utilizzabile! (carico)\n", nave_id);
    }else
        printf("Nave %d piena!\n", nave_id);
    UNLOCK
    return x;
}

void gennave()
{
    coor.x = rand() % (int)(SO_LATO + 1);
    coor.y = rand() % (int)(SO_LATO + 1);
    carico = 0;
    printf("Creata nave n. %d in posizione %f, %f\n", nave_id, coor.x, coor.y);
}

int main (int argc, char * argv[]){
    
    int mem_id;
    int nportoid;
    nave_id = atoi(argv[1]);
    srand(getpid());
    /* Ottengo l'accesso a IPC obj */
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    /*TEST ERROR*/
    gennave();
    nportoid=getnporto();
    if(nportoid != -1)
        printf("Mi dirigo verso il porto %d\nDistanza: %f\n\n", nportoid, DISTANCE(sh_mem->porti[nportoid].coord, coor));

    /*CONTINUARE*/

    sops.sem_num=1;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    sops.sem_num=2;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    shmdt ( sh_mem );
    exit(0);
}