#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>
#include <errno.h>
#include "shm.h"
#include "var.h"
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
int mem_id;
void tempesta(){
    int i;
    pid_t pid;
    while(1){
        pid = rand() % (SO_NAVI+1);
        LOCK
        if()
    }    

    
}

int main(){
    int i; char * hlp;
    srand(getpid());
    mem_id=shmget(getppid(),j,0600);
    hlp=shmat(mem_id,NULL,0600);
    sh_mem_2.porti=calloc(MERCI_RIC_OFF,struct merce);
    sh_mem.merci=(struct merce *) (hlp);
    hlp= (char *)(hlp + sizeof(struct merce)* SO_MERCI);
    sh_mem.porti = (struct porto *) (hlp);
    hlp= (char *)(hlp + sizeof(struct porto)* SO_PORTI);
    for(i=0;i<SO_PORTI;i++)
    {
        sh_mem_2.porti[i].off=(struct merce*)(hlp);
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF);
        sh_mem_2.porti[i].ric=(struct merce*) (hlp);
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF);
    }
    while(1)
    shmdt ( hlp );
    exit(0);
}