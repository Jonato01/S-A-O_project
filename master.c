#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shm.h"
#include "var.h"
#include <stdbool.h>
#include <sys/wait.h>  
#include <sys/msg.h> 
struct sembuf sops;
int sem_id; int mem_id; int banchine;
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
struct my_msg_t msg;
int msg_id;
pid_t *porti;
char* hlp;
pid_t *navi;
pid_t meteo;
void resetSems(int sem_id);
void fine_sim(int signal);
void genporti();
void gennavi();
void genmeteo();

static void msg_print_stats(int fd, int q_id) {
	struct msqid_ds my_q_data;
    msgctl(q_id, IPC_STAT, &my_q_data);
	dprintf(fd, "MASTER:\n--- IPC Message Queue ID: %8d, START ---\n", q_id);
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

void fine_sim(int signal)
{
    int n;
    for(n=0;n<SO_NAVI || n<SO_PORTI;n++){
        if(n<SO_NAVI)
            kill(navi[n],SIGINT);
        if(n<SO_PORTI)
            kill(porti[n],SIGINT);
    }
    kill(meteo, SIGINT);
    shmdt(hlp);
    shmctl(mem_id,0,IPC_RMID);
    printf("Deleting sem with id %d\n",sem_id);
    semctl(sem_id, 0, IPC_RMID);
    semctl(banchine, 0, IPC_RMID);

    printf("Deleting msg with id %d", msg_id);
    msgctl(msg_id, 0, IPC_RMID);
    exit(0);
}

void alarm_giorni(int signal)
{
    int n;
    kill(meteo, SIGUSR1);
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
    char * argsporti[]={PORTI_PATH_NAME,NULL,NULL};
    c=calloc(1,sizeof(int));
    banchine = semget(getpid()+2,SO_PORTI,0600 | IPC_CREAT);
    /*creazione porti*/
    for(i = 0; i < SO_PORTI; i++){
        
        if(!(porti[i]=fork())){
            sprintf(c, "%d", i);
            argsporti[1]=c;
            execve(PORTI_PATH_NAME,argsporti,NULL);            
            perror("Execve porti er");
	    	exit(1);
        }
        sops.sem_num=1;
        sops.sem_op=-1;
        semop(sem_id,&sops,1);
        
    }
}

void genmeteo(){
    char * argsmeteo[] = {METEO_PATH_NAME, NULL, NULL};
    if(!(meteo=fork())){
        execve(METEO_PATH_NAME, argsmeteo, NULL);
        perror("Execve meteo er");
        exit(1);
    }
}

int main(int args,char* argv[]){
    
    int i;
    struct sigaction sa;
    
    int j;
    setvar();
    j=(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF_TOT)*SO_PORTI+(sizeof(struct merce))*SO_MERCI;
    
    sh_mem_2.porti=calloc(sizeof(struct porto),SO_PORTI);
    srand(getpid());
    
    sa.sa_handler=fine_sim;
    sigaction(SIGINT, &sa, NULL);
    
    navi=calloc(SO_PORTI,sizeof(pid_t));
    porti=calloc(SO_PORTI,sizeof(pid_t));
    /*creazione IPC obj*/
    
    bzero(&sa,sizeof(sa));
    sa.sa_handler= SIG_IGN;
    sigaction(SIGALRM, &sa, NULL);
    sa.sa_handler=fine_sim;
    sigaction(SIGINT, &sa, NULL);
    sem_id = semget(getpid()+1,NUM_SEMS,0600 | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 1);
    resetSems(sem_id);
    mem_id=shmget(getpid(),j,0600 | IPC_CREAT);
    hlp=shmat(mem_id,NULL,0600);
    sh_mem.merci=(struct merce *) (hlp);
    hlp= (char *)(hlp + sizeof(struct merce) * SO_MERCI);
    sh_mem.porti = (struct porto *) ((char *) hlp);
    hlp= (char *)(hlp + sizeof(struct porto) * SO_PORTI);
    for(i=0;i<SO_PORTI;i++)
    {
        sh_mem_2.porti[i].off=(struct merce*)hlp;
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF_TOT);
        sh_mem_2.porti[i].ric=(struct merce*)hlp;
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF_TOT);
    }

    msg_id = msgget(getpid() + 3, 0600 | IPC_CREAT);

    printf("Creating shm with id: %d\nCreating sem with id:%d\nCreating msg with id:%d\n\n", 5, sem_id, msg_id);

    /*creazione merci*/
    LOCK
    
    for(i=0;i<SO_MERCI;i++)
    {
        
        sh_mem.merci[i].id=i;
        sh_mem.merci[i].size=rand()%(int)SO_SIZE+1;
        sh_mem.merci[i].vita=rand()%(int)(S0_MAX_VITA-SO_MIN_VITA+1)+SO_MIN_VITA;
        sh_mem.merci[i].num=0;
    }
    UNLOCK
    genmeteo();
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
    
    return 0;
}