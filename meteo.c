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
#include <sys/msg.h>
#include "shm.h"
#include "var.h"
struct sembuf sops;
int sem_id; int msg_id;
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
struct my_msg_t msg;
int mem_id;
char * hlp; 

void handle_morte(int signal){
    shmdt (hlp);
    exit(0);
}

void tempesta(){
    pid_t pid_nave;
    int id_nave;
    while(1){
        msgrcv(msg_id, &msg, sizeof(msg), 1, IPC_NOWAIT);
        if(errno != 0){
            switch(errno){
                case ENOMSG:
                    printf("Nessuna nave in viaggio, tempesta evitata.\n");
                    return;
                default:
                    perror("Errore nella lettura msg (tempesta 1)\n");
                    return;
            }
        }
        pid_nave = msg.pid;
        id_nave = msg.id;
        while(1){
            msgrcv(msg_id, &msg, sizeof(msg), 2, IPC_NOWAIT);
            if(errno != 0){
                switch(errno){
                    case ENOMSG:
                        printf("Tempesta scatenata su nave %d!\n", id_nave);
                        /*aggiungi segnale*/
                        return;
                    default:
                        perror("Errore nella lettura msg (tempesta 2)\n");
                        return;
                }
            }
            if(msg.id == id_nave){
                break;
            }
        }
    }
}

void handle_time(int signal){
    printf("METEO: inizia tempesta\n");
    tempesta();
}

int main(){
    int i; size_t j;
    struct sigaction sa;
    bzero(&sa,sizeof(sa));
    sa.sa_handler=handle_morte;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler=handle_time;
    sigaction(SIGUSR1, &sa, NULL);

    srand(getpid());
    j=(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF)*SO_PORTI+(sizeof(struct merce))*SO_MERCI;
    mem_id=shmget(getppid(),j,0600);
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    msg_id = msgget(getppid() +3, 0600);
    hlp=shmat(mem_id,NULL,0600);
    sh_mem_2.porti=calloc(SO_PORTI,sizeof(struct porto));
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

    while(1){
        
    }
}