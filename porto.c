
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
int porto_id;

void creaPorto(){
    /*gestire caso in cui si provi a creare più porti nelle stesse coordinate*/
}
struct merce* merciric(struct shared_data * sh_mem)
{
    
    struct merce *merci;
    int i; int x; int r;
    srand(getpid());
    merci=calloc(MERCI_RIC_DOM,sizeof(struct merce));

        
        merci[0].id=rand()%SO_MERCI;
        merci[0].vita=sh_mem-> merci[merci[0].id].vita;
        merci[0].size=sh_mem-> merci[merci[0].id].size;
        for(i=1;i<MERCI_RIC_DOM;i++)
        {
            while(1){
            r=rand()%SO_MERCI;
            for(x = 0; x < i; x++)
            {
            if(merci[x].id==r)
                break;
            }
            if(x==i){
                merci[i].id=r;
                merci[i].vita=sh_mem-> merci[merci[i].id].vita;
                merci[i].size=sh_mem-> merci[merci[i].id].size;
                merci[i].num=rand()%MAX_NUM_LOTTI+1;
                break;
            }}
    
        
        printf("creata richiesta di merce %d a porto %d\n",merci[i].id,porto_id);
    }
    return merci;




}
int main(int argc, char * argv[]){
    
    int mem_id;
    int sem_id;
    struct shared_data * sh_mem;
    struct sembuf sops;
    srand(getpid());
    porto_id = atoi(argv[1]);

    sem_id = semget(getppid()+1, NUM_SEMS, 0600);
    mem_id = shmget(getppid(), sizeof(*sh_mem), 0600);
    sh_mem = shmat(mem_id, NULL, 0);
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
            coor.x = rand() % (int)(SO_LATO + 1);
            coor.y = rand() % (int)(SO_LATO + 1);
            creaPorto();
            break;
    }
    LOCK
    sh_mem->all_ports[porto_id].x = coor.x;
    sh_mem->all_ports[porto_id].y = coor.y;
    UNLOCK
    
    printf("Creato il porto %d in posizione %f, %f\n", porto_id, coor.x, coor.y);
    merciric(sh_mem);
    sops.sem_num=1;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    sops.sem_num=2;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    exit(0);
}