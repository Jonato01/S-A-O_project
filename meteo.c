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
int sem_id; int msgN_id; int msgP_id;
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
struct my_msg_t msgN;
struct my_msg_t msgP;
int mem_id;
int msgN_id;
int msgP_id;
char * hlp; 

void handle_morte(int signal){
    printf("Ammazzando il meteo...\n");
    shmdt (hlp);
    exit(0);
}

static void msg_print_stats(int fd, int q_id) {
	struct msqid_ds my_q_data;
    msgctl(q_id, IPC_STAT, &my_q_data);
	dprintf(fd, "METEO:\n--- IPC Message Queue ID: %8d, START ---\n", q_id);
	dprintf(fd, "---------------------- Time of last msgsnd: %ld\n",
		my_q_data.msg_stime);
	dprintf(fd, "---------------------- Time of last msgrcv: %ld\n",
		my_q_data.msg_rtime);
	dprintf(fd, "---------------------- Time of last change: %ld\n",
		my_q_data.msg_ctime);
	dprintf(fd, "---------- Number of messages in the queue: %ld\n",
		my_q_data.msg_qnum);
	dprintf(fd, "- Max number of bytes allowed in the queue: %ld\n",
		my_q_data.msg_qbytes);
	dprintf(fd, "----------------------- PID of last msgsnd: %d\n",
		my_q_data.msg_lspid);
	dprintf(fd, "----------------------- PID of last msgrcv: %d\n",
		my_q_data.msg_lrpid);  
	dprintf(fd, "--- IPC Message Queue ID: %8d, END -----\n", q_id);
}

void tempesta(){
    msg_print_stats(1, msgN_id);
    if(msgrcv(msgN_id, &msgN, sizeof(msgN), 0, IPC_NOWAIT) == -1){
        if(errno == ENOMSG){
            printf("Nessuna nave in viaggio, tempesta evitata\n");
        } else {
            perror("Errore in tempesta");
        }
    } else {
        printf("Scatenata tempesta su nave %d!\n", (int) msgN.id-1);
        kill(msgN.pid, SIGUSR2);
    }
}

void mareggiata(){
    int porto; int c = 0;
    srand(getpid());
    msg_print_stats(1, msgP_id);
    porto = rand() % (SO_PORTI + 1) + 1;
    printf("Mareggiata in porto %d!\n", porto-1);
    while(1){
        if(msgrcv(msgP_id, &msgP, sizeof(msgP), porto, IPC_NOWAIT) == -1){
            if(errno != ENOMSG){
                perror("Errore in mareggiata!");
            }
            break;
        } else {    
            printf("Porto %d in mareggiata! %d\n", (int)msgP.id-1);
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
}

int main(){
    int i; size_t j;
    struct sigaction sa;
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

    while(1);
}