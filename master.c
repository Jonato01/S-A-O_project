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
int sem_id;
int mem_id;
int mem_mael;
int banchine;
struct shared_data sh_mem;
struct shared_data sh_mem_2;
struct my_msg_t msgN;
struct my_msg_t msgP;
int msgN_id;
int msgP_id;
pid_t *porti;
char *hlp;
pid_t *navi;
pid_t meteo;
int giorni;

void fine_sim(int signal)
{
    int n;
    free(sh_mem_2.porti);       
    kill(meteo, SIGINT);
    LOCK for (n = 0; n < SO_PORTI || n < SO_NAVI; n++)
    {
        if (n < SO_NAVI && navi[n] != -1)
            kill(navi[n], SIGINT);
        if (n < SO_PORTI)
            kill(porti[n], SIGINT);
    }
    free(porti); 
    UNLOCK
    sops.sem_num = 4;
    sops.sem_op = 0;
    semop(sem_id, &sops, 1);
    shmdt(navi);
    shmctl(mem_mael, 0, IPC_RMID);
    shmdt(hlp);
    shmctl(mem_id, 0, IPC_RMID);
    printf("Deleting sem with id %d\n", sem_id);
    semctl(sem_id, 0, IPC_RMID);
    semctl(banchine, 0, IPC_RMID);

    printf("Deleting msgN with id %d\n", msgN_id);
    msgctl(msgN_id, 0, IPC_RMID);

    printf("Deleting msgP with id %d\n", msgP_id);
    msgctl(msgP_id, 0, IPC_RMID);
    
    exit(0);
}

void dump(){
    int i;
    int j;
    int x0 = 0;
    int x1 = 0;
    int x2 = 0;
    int x3 = 0;
    int x4 = 0;
    int z;
    struct merce **m;
    m = calloc(SO_PORTI, sizeof(struct merce *));
    for(i = 0; i < SO_PORTI; i++){
        m[i] = calloc(MERCI_RIC_OFF*giorni, sizeof(struct merce));
    }

    LOCK
    for(i = 0; i < SO_PORTI; i++){
        for(j = 0; j < MERCI_RIC_OFF*giorni; j++){
            m[i][j] = sh_mem_2.porti[i].off[j];
        }
        printf("\n");
    }
    UNLOCK

    printf("\t   al porto\t      su nave\t   consegnate\tscadute in porto  scadute in nave\n");
    for(z = 0; z < SO_MERCI; z++){
        printf("Merce %d: ", z);
        for(i = 0; i < SO_PORTI; i++){
            for(j = 0; j < MERCI_RIC_OFF*giorni; j++){
                switch(m[i][j].status){
                    case 0:
                        x0++;
                        break;
                    case 1:
                        x1++;
                        break;
                    case 2:
                        x2++;
                        break;
                    case 3:
                        x3++;
                        break;
                    case 4:
                        x4++;
                        break;
                }
            }
        }
        printf("\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", x0, x1, x2, x3, x4);
    }

    for(i = 0; i < SO_PORTI; i++){
        free(m[i]);
    }
    free(m);
}

void alarm_giorni(int signal)
{
    int n;
    giorni++;
    LOCK
    kill(meteo, SIGUSR1);
    for (n = 0; n < SO_PORTI; n++)
    {
        kill(porti[n], SIGUSR1);
    }
    for (n = 0; n < SO_NAVI; n++)
    {
        if (navi[n] != -1)
            kill(navi[n], SIGUSR1);
    }
    UNLOCK

    dump();
}

void resetSems(int sem_id)
{
    int i;
    for (i = 1; i < NUM_SEMS; i++)
    {
        if(i==4)
        semctl(sem_id,4,SETVAL,SO_NAVI+SO_PORTI+1);
        else
        semctl(sem_id, i, SETVAL, 0);
    }
}

void shuffle(pid_t *arr, int len)
{
    int i;
    pid_t j;
    pid_t temp;
    srand(time(0));
    for (i = len - 1; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void gennavi()
{
    int i;
    pid_t h;
    char *c;
    char *argsnavi[] = {NAVI_PATH_NAME, NULL, NULL};
    c = calloc(1, sizeof(int));
    /*creazione navi*/
    for (i = 0; i < SO_NAVI; i++)
    {

        if (!(h = fork()))
        {
            sprintf(c, "%d", i);
            argsnavi[1] = c;
            free(sh_mem_2.porti);
            free(porti);   
            execve(NAVI_PATH_NAME, argsnavi, NULL);
            perror("Execve navi er");
            exit(1);
        }
        LOCK
        navi[i] = h;
        UNLOCK
        sops.sem_num = 1;
        sops.sem_op = -1;
        semop(sem_id, &sops, 1);
    }
    free(c);
}

void genporti()
{
    int i;
    char *c; pid_t j;
    char *argsporti[] = {PORTI_PATH_NAME, NULL, NULL};
    c = calloc(1, sizeof(int));
    banchine = semget(getpid() + 2, SO_PORTI, 0600 | IPC_CREAT);
    /*creazione porti*/
    for (i = 0; i < SO_PORTI; i++)
    {

        if (!(j = fork()))
        {
            sprintf(c, "%d", i);
            argsporti[1] = c;
            free(porti);
            free(sh_mem_2.porti);
            execve(PORTI_PATH_NAME, argsporti, NULL);
            perror("Execve porti er");
            exit(1);
        }
        porti[i]=j;
        sops.sem_num = 1;
        sops.sem_op = -1;
        semop(sem_id, &sops, 1);
    }
    free(c);
}

void genmeteo()
{
    char *argsmeteo[] = {METEO_PATH_NAME, NULL, NULL};
    if (!(meteo = fork()))
    {
        execve(METEO_PATH_NAME, argsmeteo, NULL);
        perror("Execve meteo er");
        exit(1);
    }
}

int main(int args, char *argv[])
{

    int i;
    struct sigaction sa;
    int j;
    setvar();
    j = (sizeof(struct porto) + sizeof(struct merce) * 2 * MERCI_RIC_OFF_TOT) * SO_PORTI + (sizeof(struct merce)) * SO_MERCI;
    sh_mem_2.porti = calloc(sizeof(struct porto), SO_PORTI);
    srand(getpid());

    sa.sa_handler = fine_sim;
    sigaction(SIGINT, &sa, NULL);
    porti = calloc(SO_PORTI, sizeof(pid_t));
    /*creazione IPC obj*/
    giorni = 0;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGALRM, &sa, NULL);
    sa.sa_handler = fine_sim;
    sigaction(SIGINT, &sa, NULL);
    sem_id = semget(getpid() + 1, NUM_SEMS, 0600 | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 1);
    resetSems(sem_id);
    mem_id = shmget(getpid(), j, 0600 | IPC_CREAT);
    mem_mael = shmget(getpid() + 9, sizeof(pid_t) * so_navi, 0600 | IPC_CREAT);
    navi = shmat(mem_mael, NULL, 0600);
    hlp = shmat(mem_id, NULL, 0600);
    sh_mem.merci = (struct merce *)(hlp);
    hlp = (char *)(hlp + sizeof(struct merce) * SO_MERCI);
    sh_mem.porti = (struct porto *)((char *)hlp);
    hlp = (char *)(hlp + sizeof(struct porto) * SO_PORTI);
    for (i = 0; i < SO_PORTI; i++)
    {
        sh_mem_2.porti[i].off = (struct merce *)hlp;
        hlp = (char *)(hlp + sizeof(struct merce) * MERCI_RIC_OFF_TOT);
        sh_mem_2.porti[i].ric = (struct merce *)hlp;
        hlp = (char *)(hlp + sizeof(struct merce) * MERCI_RIC_OFF_TOT);
    }

    msgN_id = msgget(getpid() + 3, 0600 | IPC_CREAT);
    msgP_id = msgget(getpid() + 4, 0600 | IPC_CREAT);

    printf("Creating shm with id: %d\nCreating sem with id:%d\nCreating msgN with id:%d\nCreating msgP with id:%d\n", 5, sem_id, msgN_id, msgP_id);

    /*creazione merci*/
    LOCK for (i = 0; i < SO_MERCI; i++)
    {

        sh_mem.merci[i].id = i + 1;
        sh_mem.merci[i].size = rand() % SO_SIZE + 1;
        sh_mem.merci[i].vita = rand() % (S0_MAX_VITA - SO_MIN_VITA + 1) + SO_MIN_VITA;
        sh_mem.merci[i].num = 0;
    }
    UNLOCK
    genporti();
    gennavi();
    genmeteo();
    sops.sem_num = 2;            
    sops.sem_op = SO_NAVI+1;            
    semop(sem_id,&sops, 1);
    sa.sa_handler = alarm_giorni;
    sigaction(SIGALRM, &sa, NULL);
    sa.sa_handler = fine_sim;
    sigaction(SIGINT, &sa, NULL);
    semctl(sem_id, 2, SETVAL, 0);
    for (i = 0; i < SO_GIORNI - 1; i++)
    {
        sleep(1);
        raise(SIGALRM);
    }
    sleep(1);
    raise(SIGINT);

    return 0;
}