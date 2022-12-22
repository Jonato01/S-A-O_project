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

struct sembuf sops;
struct shared_data * sh_mem;
int sem_id;
struct merce *merci_ric;
bool empty = true;

struct nave barchetta;

int ord[SO_PORTI]; /* ID porti in ordine di distanza*/
void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void ordinaporti(struct coordinates coord){
    int i;
    int j;
    bool swapped;
    for(i = 0; i < SO_PORTI; i++){
        ord[i] = sh_mem->porti[i].idp;
    }
    for (i = 0; i < SO_PORTI - 1; i++){
        swapped = false;
        for (j = 0; j < SO_PORTI - i - 1; j++)
            if (DISTANCE(sh_mem->porti[ord[j]].coord, coord) > DISTANCE(sh_mem->porti[ord[j+1]].coord, coord)){
                swap(&ord[j], &ord[j + 1]);
                swapped = true;
            }
        if(!swapped)
            break;
    }
    /*for(i = 0; i < SO_PORTI; i++){
        printf("%d ", ord[i]);
    }
    printf("\n");*/
}

int contains(int portoid, int merceid){
    int i;

    for(i = 0; i < MERCI_RIC_OFF; i++){
        if(sh_mem->porti[portoid].off[i].id == merceid){
            return i;
        }
    }
    return -1;
}

int getpart()
{
    int x = -1;
    int y;
    int i;
    int j;
    bool flag = false;
    LOCK   
    for(i = 0; i < SO_PORTI; i++){
        for(j = 0; j < MERCI_RIC_OFF; j++){
                y = contains(ord[i], merci_ric[j].id);
                if((y > -1) && !sh_mem->porti[ord[i]].off[y].pre && merci_ric[j].pre){
                    flag = true;
                    sh_mem->porti[ord[i]].off[y].pre = true;
                    printf("Nave %d: prenotata merce %d da caricare al porto %d (carico)\n", barchetta.idn, sh_mem->porti[ord[i]].off[y].id, ord[i]);
                }
            }
        
        if(flag){
            x = ord[i];
            break;
        }
    }
    if(!flag){
        printf("Nave %d: nessun porto utilizzabile! (carico)\n", barchetta.idn);
    }     
    UNLOCK
    printf("\n");
    return x;
}

int getdest()
{
    int x = -1;
    int i;
    int j;
    bool flag = false;
    LOCK
    if(barchetta.carico_pre < SO_CAPACITY){
        for(i = 0; i < SO_PORTI; i++){
            for(j = 0; j < MERCI_RIC_OFF && sh_mem->porti[ord[i]].ric[j].size + barchetta.carico_pre < SO_CAPACITY; j++){
                if(!sh_mem->porti[ord[i]].ric[j].pre){
                    flag = true;
                    sh_mem->porti[ord[i]].ric[j].pre = true;
            /*!!!j*/merci_ric[j]=sh_mem->porti[ord[i]].ric[j];
                    merci_ric[j].pre = true;
                    barchetta.carico_pre += sh_mem->porti[ord[i]].ric[j].size;
                    printf("Nave %d: prenotata merce %d da consegnare al porto %d (scarico)\n", barchetta.idn, sh_mem->porti[ord[i]].ric[j].id, ord[i]);
                }
            }
            
            if(flag){
                x = ord[i];
                break;
            }
        }
        if(!flag)
            printf("Nave %d: nessun porto utilizzabile! (scarico)\n", barchetta.idn);
        else
            printf("Totale carico: %f su %d\n", barchetta.carico_pre, SO_CAPACITY);
    }else
        printf("Nave %d piena!\n", barchetta.idn);
    UNLOCK
    return x;
}

void gennave()
{
    barchetta.coord.x = rand() % (int)(SO_LATO + 1);
    barchetta.coord.y = rand() % (int)(SO_LATO + 1);
    barchetta.carico = 0;
    barchetta.carico_pre = 0;
    printf("Creata nave n. %d in posizione %f, %f\n", barchetta.idn, barchetta.coord.x, barchetta.coord.y);
}

int main (int argc, char * argv[]){
    
    int mem_id;
    merci_ric=calloc(MERCI_RIC_OFF,sizeof(struct merce));    
    barchetta.idn= atoi(argv[1]);
    srand(getpid());
    /* Ottengo l'accesso a IPC obj */
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    /*TEST ERROR*/

    gennave();

    ordinaporti(barchetta.coord);
    barchetta.idp_dest = getdest();
    ordinaporti(sh_mem->porti[barchetta.idp_dest].coord);
    barchetta.idp_part = getpart();
    if(barchetta.idp_part != -1)
        printf("Mi dirigo verso il porto %d\nDistanza: %f\n\n", barchetta.idp_part, DISTANCE(sh_mem->porti[barchetta.idp_part].coord, barchetta.coord));

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