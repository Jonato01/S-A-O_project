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
#include <stdbool.h>
#include "shm.h"
#include "var.h"
struct sembuf sops;
int sem_id; int msgN_id; int msgP_id;
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
struct my_msg_t msgN;
struct my_msg_t msgP;
struct timespec rem;pid_t* navi;  int mem_mael;
int mem_id;
int msgN_id;
int msgP_id; 
char * hlp; 
bool flag;

void handle_morte(int signal){
    printf("Ammazzando il meteo...\n");
    shmdt (hlp);
    shmdt(navi);
    exit(0);
}

void tempesta(){
    do{
        if(msgrcv(msgN_id, &msgN, sizeof(msgN), 0, IPC_NOWAIT) == -1){
            if(errno == ENOMSG){
                printf("Nessuna nave in viaggio, tempesta evitata\n");
            } else if(errno!=EINTR) {
                perror("Errore in tempesta");
            }
        } else {
            printf("Scatenata tempesta su nave %d!\n", (int) msgN.id-1);
            kill(msgN.pid, SIGUSR2);
        }
    }while(errno==EINTR); 
}

void mareggiata(){
    int porto; int c = 0;
    srand(getpid());
    porto = rand() % (SO_PORTI + 1) + 1;
    printf("Mareggiata in porto %d!\n", porto-1);
    while(1){
        if(msgrcv(msgP_id, &msgP, sizeof(msgP), porto, IPC_NOWAIT) == -1){
            if(errno != ENOMSG && errno!=EINTR){
                perror("Errore in mareggiata!");
            }
            break;
        } else {    
            printf("Porto %d in mareggiata!\n", (int)msgP.id-1);
            kill(msgP.pid, SIGWINCH /* consigliato da chatGPT come SIGUSR3*/);
            c++;
        }
    }
    printf("METEO: colpite %d navi dalla mareggiata\n", c);
}

void handle_time(int signal){
    printf("METEO: inizia tempesta\n");
    tempesta();
    printf("METEO: inizia mareggiata\n");
    mareggiata();
    nanosleep(&rem, &rem);
}



int main(){
    int i; size_t j;
    double x, fractpart, intpart;
    struct sigaction sa; int navi_vita;
    
    flag = true;
    setvar();
    bzero(&sa,sizeof(sa));
    sa.sa_handler=handle_time;
    sigaction(SIGUSR1, &sa, NULL);
    srand(getpid());
    j=(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF)*SO_PORTI+(sizeof(struct merce))*SO_MERCI;
    mem_id=shmget(getppid(),j,0600);
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    msgN_id = msgget(getppid() +3, 0600);
    printf("Meteo: msgN_id: %d\n", msgN_id);
    msgP_id = msgget(getppid() +4, 0600);
    printf("Meteo: msgP_id: %d\n", msgP_id);
    
    mem_mael=shmget(getppid()+9,sizeof(pid_t)*SO_NAVI,0600);
    navi=shmat(mem_mael,NULL,0600);
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
    sa.sa_handler=handle_morte;
    sigaction(SIGINT, &sa, NULL);
    navi_vita=SO_NAVI;
    x = (double) SO_MAELSTROM / 24;
            fractpart=modf(x,&intpart);
    while(1){
        if(flag){
            
            rem.tv_sec = intpart;
            rem.tv_nsec = fractpart*1e9;
            nanosleep(&rem, &rem);
            printf("METEO: inizia vortice\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
            LOCK
            do{
            i=rand()%SO_NAVI;
            }while(navi[i]==-1);   
            UNLOCK
            printf("Nave  colpita dal vortice!! pid = %d\n\n",navi[i]);
            LOCK
            kill(navi[i], SIGINT);
            navi[i]=-1;
            TEST_ERROR
            UNLOCK
            navi_vita--;
             if(!navi_vita){
            printf("Non ci sono più navi!\n");
            flag = false;
            }
            
        }else pause();
    }
}