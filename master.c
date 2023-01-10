#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/sem.h>
#include "shm.h"
#include "var.h"
#include <stdbool.h>
#include <sys/wait.h>   
struct sembuf sops;
int sem_id; int mem_id; int banchine;
struct shared_data * sh_mem;

pid_t *porti;
pid_t *navi;
pid_t meteo;
void resetSems(int sem_id);
void fine_sim(int signal);
void genporti();
void gennavi();
void fine_sim(int signal)
{
    int n;
    for(n=0;n<SO_NAVI || n<SO_PORTI;n++){
        if(n<SO_NAVI)
        kill(navi[n],SIGINT);
        if(n<SO_PORTI)
        kill(porti[n],SIGINT);
    }


    shmdt ( sh_mem );
    printf("\nDeleting shm with id %d\n",mem_id);
    shmctl(mem_id , IPC_RMID , NULL);
    printf("Deleting sem with id %d\n",sem_id);
    semctl(sem_id, 0, IPC_RMID);
    semctl(banchine, 0, IPC_RMID);
    exit(0);
}
void alarm_giorni(int signal)
{
    int n;
    for(n=0;n<SO_NAVI || n<SO_PORTI;n++){
            if(n<SO_NAVI)
            kill(navi[n],SIGUSR1);
            if(n<SO_PORTI)
            kill(porti[n],SIGUSR1);
        }
}
void resetSems(int sem_id){
    int i;
    for(i = 1; i < NUM_SEMS; i++){
        semctl(sem_id, i, SETVAL, 0);
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
        if(!(navi[i]=fork())){
            sprintf(c, "%d", i);
            argsnavi[1]=c;
            execve(NAVI_PATH_NAME,argsnavi,NULL);
            perror("Execve navi er");
	    	exit(1);
        }
        sops.sem_num=1;
        sops.sem_op=-1;
        semop(sem_id,&sops,1);
    }
}

void genporti()
{
    
    int i;
    char *c;
    char * argsnavi[]={PORTI_PATH_NAME,NULL,NULL};
    c=calloc(1,sizeof(int));
    banchine = semget(getpid()+2,SO_PORTI,0600 | IPC_CREAT);
    /*creazione porti*/
    for(i = 0; i < SO_PORTI; i++){
        
        if(!(porti[i]=fork())){
            sprintf(c, "%d", i);
            argsnavi[1]=c;

            execve(PORTI_PATH_NAME,argsnavi,NULL);            
            perror("Execve porti er");
	    	exit(1);
        }
        sops.sem_num=1;
        sops.sem_op=-1;
        semop(sem_id,&sops,1);
        
    }
}

int main(int args,char* argv[]){
    
    int i; void *sh;
    struct sigaction sa;
    srand(getpid());
    /*setvar();*/
    printf("%d %d %d\n", SO_NAVI, SO_PORTI, SO_MERCI);
    
    
    sh_mem=calloc(1,sizeof(struct shared_data));
    sh_mem->merci=calloc(SO_MERCI,sizeof(struct merce));
    for(i=0;i<SO_MERCI;i++)
    {
        sh_mem->merci->pid_navi=calloc(SO_NAVI,sizeof(pid_t));
    }
    sh_mem->porti=calloc(SO_PORTI,sizeof(struct porto));
    for(i=0;i<SO_PORTI;i++)
    {
        sh_mem->porti->ric=calloc(MERCI_RIC_OFF,sizeof(struct merce));
        sh_mem->porti->off=calloc(MERCI_RIC_OFF,sizeof(struct merce));
    }
    sh_mem->navi_in_transito=calloc(SO_NAVI,sizeof(pid_t));
    
    navi=calloc(SO_PORTI,sizeof(pid_t));
    porti=calloc(SO_PORTI,sizeof(pid_t));
    /*creazione IPC obj*/
    
    bzero(&sa,sizeof(sa));
    sa.sa_handler= SIG_IGN;
    sigaction(SIGALRM, &sa, NULL);
    sem_id = semget(getpid()+1,NUM_SEMS,0600 | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 1);
    resetSems(sem_id);
    mem_id = shmget (getpid(),sizeof(struct shared_data)+(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF)*SO_PORTI+(sizeof(struct merce))*SO_MERCI+sizeof(pid_t)*SO_NAVI, 0600 | IPC_CREAT );
    sh = shmat(mem_id, NULL, 0);
    sh_mem=(struct shared_data*)sh;
    sh+=sizeof(struct shared_data);
    sh_mem->merci=(struct merce*) sh;
    sh+=sizeof(struct merce)*SO_MERCI;
    sh_mem->navi_in_transito=(pid_t*) sh;
    sh+=sizeof(pid_t*)*SO_NAVI;
    sh_mem->porti=(struct porto*)sh;
    sh+=sizeof(struct porto)*SO_PORTI;
    for(i=0;i<SO_PORTI;i++)
    {
        sh_mem->porti[i].ric=(struct merce*)sh;
        sh+=sizeof(struct merce)*MERCI_RIC_OFF;
        sh_mem->porti[i].off=(struct merce*)sh;
        sh+=sizeof(struct merce)*MERCI_RIC_OFF;
    }    
    printf("Creating shm with id: %d\nCreating sem with id:%d\n\n", mem_id, sem_id);
    /*creazione merci*/
    LOCK
    for(i=0;i<SO_MERCI;i++)
    {
        sh_mem-> merci[i].id=i;
        sh_mem-> merci[i].size=rand()%(int)SO_SIZE+1;
        sh_mem-> merci[i].vita=rand()%(int)(S0_MAX_VITA-SO_MIN_VITA+1)+SO_MIN_VITA;
        sh_mem-> merci[i].num=0;
    }
    
    UNLOCK
    printf("ciao");
    genporti();     
    
    gennavi();
    sops.sem_num = 2;            
    sops.sem_op = SO_NAVI+1;            
    semop(sem_id,&sops, 1);
    sa.sa_handler=alarm_giorni;
    sigaction(SIGALRM, &sa, NULL);
    sa.sa_handler=fine_sim;
    sigaction(SIGINT, &sa, NULL);
    
    
    for(i=0;i<=SO_GIORNI;i++){
        
        alarm(1);
        sleep(1);    
    }
    raise(SIGINT);
    

}