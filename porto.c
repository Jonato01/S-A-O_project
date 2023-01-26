#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "shm.h"
#include "var.h"
struct sembuf sops;
struct shared_data sh_mem;
struct shared_data sh_mem_2;
struct coordinates coor;
char *hlp;
int sem_id;
int porto_id;
int giorno;
void genric();
void genmerci();
void handle_morte(int signal)
{
    giorno++;
    free(sh_mem_2.porti);
    LOCK
    sops.sem_num = 2;
    sops.sem_op = 1;
    semop(sem_id, &sops, 1);
    UNLOCK
    shmdt(hlp);
    exit(0);
}

void handle_time(int signal)
{
    int i = 0;

    giorno++;
    LOCK

        for (i = 0; i < MERCI_RIC_OFF_TOT && sh_mem_2.porti[porto_id].off[i].size; i++)
    {
        if (!sh_mem_2.porti[porto_id].off[i].vita && !sh_mem_2.porti[porto_id].off[i].num)
        {
            sh_mem_2.porti[porto_id].off[i].vita--;
            if (sh_mem_2.porti[porto_id].off[i].vita == 0)
            {
                bzero(&sh_mem_2.porti[porto_id].off[i], sizeof(struct merce));
                sh_mem_2.porti[porto_id].off[i].size = -1;
                /*Far scadere bene le merci*/
            }
        }
    }

    genmerci();
    genric();
    UNLOCK
    sops.sem_num = 2;
    sops.sem_op = 1;
    semop(sem_id, &sops, 1);
}

void creaPorto()
{
    /*gestire caso in cui si provi a creare più porti nelle stesse coordinate*/
    int i;

    bool flag = false;
    do
    {
        coor.x = rand() % (int)(SO_LATO + 1);
        coor.y = rand() % (int)(SO_LATO + 1);

        for (i = 0; i < porto_id; i++)
        {
            if (coor.x == sh_mem.porti[i].coord.x && coor.y == sh_mem.porti[i].coord.y)
            {
                flag = true;
                break;
            }
            else
            {
                flag = false;
            }
        }
    } while (flag);
}

void genric()
{
    int j;
    int i;
    int x;
    int r;
    bool off;
    srand(getpid());
    for (i = MERCI_RIC_OFF * giorno; i < (MERCI_RIC_OFF + MERCI_RIC_OFF * giorno); i++)
    {
        while (1)
        {
            r = rand() % SO_MERCI + 1;
            for (x = MERCI_RIC_OFF * giorno; x < i; x++)
            {
                if (sh_mem_2.porti[porto_id].ric[x].id == r)
                    break;
            }
            off = true;
            for (j = 0; j < (MERCI_RIC_OFF + MERCI_RIC_OFF * giorno) && off; j++)
            {
                if (r == sh_mem_2.porti[porto_id].off[j].id)
                    off = false;
            }
            if (x == i && off)
            {
                sh_mem_2.porti[porto_id].ric[x].id = r;
                sh_mem_2.porti[porto_id].ric[x].vita = sh_mem.merci[r - 1].vita;
                sh_mem_2.porti[porto_id].ric[x].size = sh_mem.merci[r - 1].size;
                sh_mem_2.porti[porto_id].ric[x].pre = false;
                r = (SO_FILL / SO_PORTI / MERCI_RIC_OFF_TOT / SO_GIORNI) / sh_mem_2.porti[porto_id].ric[x].size;
                if ((SO_FILL / SO_PORTI / MERCI_RIC_OFF_TOT / SO_GIORNI) % sh_mem_2.porti[porto_id].ric[x].size)
                    r++;
                sh_mem_2.porti[porto_id].ric[x].num = r;
                break;
            }
        }

        printf("creata domanda di %d lotti da %d ton di merce %d a porto %d\n", sh_mem_2.porti[porto_id].ric[x].num, sh_mem.merci[sh_mem_2.porti[porto_id].ric[x].id - 1].size, sh_mem_2.porti[porto_id].ric[i].id, porto_id);
    }
    printf("\n");
}

void genmerci()
{
    int i;
    int x;
    int r;
    int j;
    bool ric;
    srand(getpid());
    for (i = MERCI_RIC_OFF * giorno; i < (MERCI_RIC_OFF + MERCI_RIC_OFF * giorno); i++)
    {
        while (1)
        {
            r = rand() % SO_MERCI + 1;
            for (x = MERCI_RIC_OFF * giorno; x < i; x++)
            {
                if (sh_mem_2.porti[porto_id].off[x].id == r)
                    break;
            }
            ric = true;
            for (j = 0; j < (MERCI_RIC_OFF * giorno) && ric; j++)
            {
                if (r == sh_mem_2.porti[porto_id].ric[j].id && sh_mem_2.porti[porto_id].ric[j].num)
                    ric = false;
            }
            if (x == i && ric)
            {
                sh_mem_2.porti[porto_id].off[x].id = r;
                sh_mem_2.porti[porto_id].off[x].vita = sh_mem.merci[r - 1].vita;
                sh_mem_2.porti[porto_id].off[x].size = sh_mem.merci[r - 1].size;
                sh_mem_2.porti[porto_id].off[x].pre = 0;

                r = (SO_FILL / SO_PORTI / MERCI_RIC_OFF_TOT / SO_GIORNI) / sh_mem_2.porti[porto_id].off[x].size;
                if (r == 0)
                    r++;
                sh_mem_2.porti[porto_id].off[x].num = r;
                sh_mem_2.porti[porto_id].off[x].status = 0;
                break;
            }
        }
        printf("creati %d lotti da %d ton di merce %d a porto %d (vita: %d giorni)\n", sh_mem_2.porti[porto_id].off[x].num, sh_mem.merci[sh_mem_2.porti[porto_id].off[x].id - 1].size, sh_mem_2.porti[porto_id].off[i].id, porto_id, sh_mem_2.porti[porto_id].off[i].vita);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int maxbanchine;
    int mem_id;
    int i;
    int bancid;
    struct sigaction sa;
    size_t j;
    setvar();
    j = (sizeof(struct porto) + sizeof(struct merce) * 2 * MERCI_RIC_OFF_TOT) * SO_PORTI + (sizeof(struct merce)) * SO_MERCI;
    srand(getpid());
    sh_mem_2.porti = calloc(SO_PORTI, sizeof(struct porto));

    bzero(&sa, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    porto_id = atoi(argv[1]);
    bancid = semget(getppid() + 2, SO_PORTI, 0600);
    sem_id = semget(getppid() + 1, NUM_SEMS, 0600);

    mem_id = shmget(getppid(), j, 0600);
    hlp = shmat(mem_id, NULL, 0600);

    sh_mem.merci = (struct merce *)(hlp);
    hlp = (char *)(hlp + sizeof(struct merce) * SO_MERCI);
    sh_mem.porti = (struct porto *)(hlp);
    hlp = (char *)(hlp + sizeof(struct porto) * SO_PORTI);
    for (i = 0; i < SO_PORTI; i++)
    {
        sh_mem_2.porti[i].off = (struct merce *)(hlp);
        hlp = (char *)(hlp + sizeof(struct merce) * MERCI_RIC_OFF_TOT);
        sh_mem_2.porti[i].ric = (struct merce *)(hlp);
        hlp = (char *)(hlp + sizeof(struct merce) * MERCI_RIC_OFF_TOT);
    }
    LOCK
        sh_mem.porti[porto_id]
            .idp = porto_id;
    UNLOCK
    /*TEST ERROR*/
    switch (porto_id)
    {
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
    giorno = 0;
    LOCK
        sh_mem.porti[porto_id]
            .coord.x = coor.x;
    sh_mem.porti[porto_id].coord.y = coor.y;
    maxbanchine = rand() % SO_BANCHINE + 1;
    semctl(bancid, porto_id, SETVAL, maxbanchine);
    printf("Creato il porto %d in posizione %f, %f, con %d banchine\n", porto_id, coor.x, coor.y, maxbanchine);

    genmerci();
    genric();
    UNLOCK
    sa.sa_handler = handle_time;
    sigaction(SIGUSR1, &sa, NULL);

    sops.sem_num = 1;
    sops.sem_op = 1;
    semop(sem_id, &sops, 1);
    sa.sa_handler = handle_morte;
    sigaction(SIGINT, &sa, NULL);
    while (1)
    {
        pause();
        TEST_ERROR
    }
}