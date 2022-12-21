
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

struct shared_data * sh_mem;
struct coordinates coor;
int porto_id;

void creaPorto(){
    /*gestire caso in cui si provi a creare più porti nelle stesse coordinate*/
    int i;
    bool flag = false;   
    do{
        coor.x = rand() % (int)(SO_LATO + 1);
        coor.y = rand() % (int)(SO_LATO + 1);

        for(i = 0; i < porto_id; i++){
            if(coor.x == sh_mem->porti[i].coord.x && coor.y == sh_mem->porti[i].coord.y){
                flag = true;
                break;
            } else {
                flag = false;
            }
        }
    }while(flag);
}

void genric()
{
    int j;int i; int x; int r;
    bool off;
     
    srand(getpid());
    for(i=0;i<MERCI_RIC_OFF;i++)
    {
        while(1){
        r=rand()%SO_MERCI;
            for(x = 0; x < i; x++)
            {
                if(sh_mem->porti[porto_id].ric[x].id==r)
                break;    
            }
            off=true;
            for(j=0; j<MERCI_RIC_OFF && off;j++)
            {
                    if(r==sh_mem->porti[porto_id].off[j].id)
                    off=false;
            }
            if(x==i && off){
            sh_mem->porti[porto_id].ric[x].id=r;
            sh_mem->porti[porto_id].ric[x].vita=sh_mem-> merci[sh_mem->porti[porto_id].ric[x].id].vita;
            sh_mem->porti[porto_id].ric[x].size=sh_mem-> merci[sh_mem->porti[porto_id].ric[x].id].size;
            sh_mem->porti[porto_id].ric[x].pre=false;
            sh_mem->porti[porto_id].ric[x].num=rand()%MAX_NUM_LOTTI+1;
            break;
            }
        }
        
        printf("creata domanda di %d ton di merce %d a porto %d\n", sh_mem->merci[sh_mem->porti[porto_id].ric[x].id].size, sh_mem->porti[porto_id].ric[i].id,porto_id);
    }
    printf("\n");
    
}

void genmerci()
{
    int i; int x; int r; 
    srand(getpid());
    for(i=0;i<MERCI_RIC_OFF;i++)
    {
        while(1){
        r=rand()%SO_MERCI;
            for(x = 0; x < i; x++)
            {
                if(sh_mem->porti[porto_id].off[x].id==r)
                break;
            }
            if(x==i){
            sh_mem->porti[porto_id].off[x].id=r;
            sh_mem->porti[porto_id].off[x].vita=sh_mem-> merci[sh_mem->porti[porto_id].off[x].id].vita;
            sh_mem->porti[porto_id].off[x].size=sh_mem-> merci[sh_mem->porti[porto_id].off[x].id].size;
            sh_mem->porti[porto_id].off[x].pre=false;
            sh_mem->porti[porto_id].off[x].num=rand()%MAX_NUM_LOTTI+1;
            break;
            }
        }        
        printf("create %d ton di merce %d a porto %d\n",sh_mem-> merci[sh_mem->porti[porto_id].off[x].id].size, sh_mem->porti[porto_id].off[i].id,porto_id);
    }
    printf("\n");
    
} 

int main(int argc, char * argv[]){
    
    int mem_id;
    int sem_id;
   
    struct sembuf sops;
    srand(getpid());
    porto_id = atoi(argv[1]);

    sem_id = semget(getppid()+1, NUM_SEMS, 0600);
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
    LOCK
    sh_mem->porti[porto_id].idp=porto_id;
    UNLOCK
    /*TEST ERROR*/
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
            creaPorto();
            break;
    }
    
    LOCK
    sh_mem->porti[porto_id].coord.x = coor.x;
    sh_mem->porti[porto_id].coord.y = coor.y;
    sh_mem->porti[porto_id].maxbanchine = rand() % SO_BANCHINE + 1;
    sh_mem->porti[porto_id].banchinelibere = sh_mem->porti[porto_id].maxbanchine;

    printf("Creato il porto %d in posizione %f, %f, con %d banchine\n", porto_id, coor.x, coor.y, sh_mem->porti[porto_id].maxbanchine);

    genmerci();
    genric();
    UNLOCK

    sops.sem_num=1;
    sops.sem_op=1;
    semop(sem_id,&sops,1);

    sops.sem_num=2;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    shmdt ( sh_mem );
    exit(0);
}