#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/msg.h> 
#include <stdbool.h>
#include <signal.h>
#include <strings.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <math.h>
#include <errno.h>
#include "shm.h"

struct sembuf sops;
struct shared_data  sh_mem;
struct shared_data  sh_mem_2;
struct my_msg_t  msgN;
struct my_msg_t msgP;
int sem_id; int msgN_id; int msgP_id;
struct merce *merci_ric;
char* hlp;
int nmerci;
int bancid;
struct nave barchetta;
struct timespec rem;
bool travelling; bool working; bool waiting_storm; bool waiting_swell; bool waiting_day;
bool msgNf; bool msgPf1; bool msgPf2;
int *ord; /* ID porti in ordine di distanza*/

void handle_morte(int signal){
    LOCK
    sops.sem_num = 2;            
    sops.sem_op = 1;            
    semop(sem_id,&sops, 1);
    UNLOCK
    if(msgNf){
        printf("La nave %d era in viaggio\n", barchetta.idn);
        if(msgrcv(msgN_id, &msgN, sizeof(msgN), barchetta.idn+1, IPC_NOWAIT) == -1){
            if(errno != ENOMSG)
                perror("Errore in handle_morte 1");
        } else {
            printf("Nave %d: rimossa dalla coda\n",barchetta.idn);
        }
    }
    if(msgPf1){
        printf("La nave %d era al porto\n", barchetta.idn);
        if(msgrcv(msgP_id, &msgP, sizeof(msgP), barchetta.idp_part+1, IPC_NOWAIT) == -1){
            if(errno != ENOMSG)
                perror("Errore in handle_morte 2");
        } else {
            printf("Nave %d: rimosso porto %d dalla coda\n",barchetta.idn, barchetta.idp_part);
        }
    }
    if(msgPf2){
        printf("La nave %d era al porto\n", barchetta.idn);
        if(msgrcv(msgP_id, &msgP, sizeof(msgP), barchetta.idp_dest+1, IPC_NOWAIT) == -1){
            if(errno != ENOMSG)
                perror("Errore in handle_morte 2");
        } else {
            printf("Nave %d: rimosso porto %d dalla coda\n",barchetta.idn, barchetta.idp_dest);
        }
    }
    printf("La nave %d ha finito di vivere\n", barchetta.idn);
    shmdt (hlp);
    exit(0);
}

void handle_time(int signal)
{
    int i;
    if(signal == SIGUSR1){
        for(i=0; i<SO_MERCI;i++)
        {
            if(merci_ric[i].id != -1 && merci_ric[i].status == 1){
                merci_ric[i].vita--;
                if(merci_ric[i].vita<=0){
                    printf("Nave %d: merce %d scaduta!\n", barchetta.idn, merci_ric[i].id);
                    barchetta.carico -= merci_ric[i].size * merci_ric[i].pre;
                    merci_ric[i].num = 0;
                    merci_ric[i].status = 0;
                }
            }
        }
        if(travelling || working || waiting_storm || waiting_swell){
            printf("Oh no! Anyway...\n");
            nanosleep(&rem, &rem);
        }
    }
}

void handle_storm(int signal){
    int nano; double q;

    if(travelling){
        printf("Nave %d: Tempesta mentre viaggiavo\n", barchetta.idn);
        nanosleep(&rem, &rem);
        travelling = false;
    }

    q = (double) SO_STORM_DURATION;
    nano=modf(q,&q);
    rem.tv_sec = q;
    rem.tv_nsec = nano*1e9;

    waiting_storm = true;
    printf("Nave %d, in tempesta per %d secondi\n", barchetta.idn, SO_STORM_DURATION);
    nanosleep(&rem, &rem);
    waiting_storm = false;

    printf("Nave %d: tempesta passata\n\n", barchetta.idn);
}

void handle_swell(int signal){
    int nano; double q;

    if(working){
        printf("Nave %d: Mareggiata mentre caricavo/scaricavo\n", barchetta.idn);
        nanosleep(&rem, &rem);
        working = false;
    }

    q = (double) SO_SWELL_DURATION;
    nano=modf(q,&q);
    rem.tv_sec = q;
    rem.tv_nsec = nano*1e9;

    waiting_swell = true;
    printf("Nave %d, porto in mareggiata per %d\n", barchetta.idn, SO_SWELL_DURATION);
    nanosleep(&rem, &rem);
    waiting_swell = false;

    printf("Nave %d: mareggiata passata\n\n", barchetta.idn);
}

void swap(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void ordinaporti(struct coordinates coord){
    int i;
    int j;
    bool swapped;
    for(i = 0; i < SO_PORTI; i++){
        ord[i] = sh_mem.porti[i].idp;
    }
    for (i = 0; i < SO_PORTI - 1; i++){
        swapped = false;
        for (j = 0; j < SO_PORTI - i - 1; j++)
        
            if (DISTANCE(sh_mem.porti[ord[j]].coord, coord) > DISTANCE(sh_mem.porti[ord[j+1]].coord, coord)){
                swap(&ord[j], &ord[j + 1]);
                swapped = true;
            }
        if(!swapped)
            break;
    }
}

int containsOff(int portoid, int merceid){
    int i;
    for(i = 0; i < MERCI_RIC_OFF; i++){
        if(sh_mem_2.porti[portoid].off[i].id == merceid && sh_mem_2.porti[portoid].off[i].num){
            return i;
        }
    }
    return -1;
}

int containsRic(int portoid, int merceid){
    int i;
    for(i = 0; i < MERCI_RIC_OFF; i++){
        if(sh_mem_2.porti[portoid].ric[i].id == merceid && sh_mem_2.porti[portoid].ric[i].num){
            return i;
        }
    }
    return -1;
}

int getpart()
{
    int x = -1;
    int y;
    int i;
    int j;
    int q;
    bool flag = false;
    LOCK
    for(i = 0; i < SO_PORTI; i++){
        for(j = 0; j < MERCI_RIC_OFF; j++){
            y = containsOff(ord[i], merci_ric[j].id);
            if((y > -1) && (sh_mem_2.porti[ord[i]].off[y].num - sh_mem_2.porti[ord[i]].off[y].pre > 0) && (merci_ric[j].pre)){
                flag = true;
                q = merci_ric[j].pre;
                while(q > 0 && sh_mem_2.porti[ord[i]].off[y].num - sh_mem_2.porti[ord[i]].off[y].pre > 0){
                    sh_mem_2.porti[ord[i]].off[y].pre++;
                    q--;
                }
                q = merci_ric[j].pre - q;
                printf("Nave %d: prenotati %d lotti di merce %d su %d da caricare al porto %d (carico)\n", barchetta.idn, q, sh_mem_2.porti[ord[i]].off[y].id, merci_ric[j].pre, ord[i]);
                merci_ric[j].pre = q;
            }
        }
        
        if(flag){
            x = ord[i];
            break;
        }
    }
    if(!flag){
        printf("Nave %d: nessun porto utilizzabile! (carico)\n", barchetta.idn);
    }     
    UNLOCK
    return x;
}

int getdest()
{
    int x = -1;
    int i;
    int j;
    bool flag = false;
    ordinaporti(barchetta.coord);
    
    LOCK
    if(barchetta.carico_pre < SO_CAPACITY){
        
        for(i = 0; i < SO_PORTI; i++){
            for(j = 0; j < MERCI_RIC_OFF; j++){
                if(!sh_mem_2.porti[ord[i]].ric[j].pre && sh_mem_2.porti[ord[i]].ric[j].size + barchetta.carico_pre <= SO_CAPACITY){
                    flag = true;
                    merci_ric[j]=sh_mem_2.porti[ord[i]].ric[j];
                    merci_ric[j].num = 0;
                    while(sh_mem_2.porti[ord[i]].ric[j].pre < sh_mem_2.porti[ord[i]].ric[j].num && sh_mem_2.porti[ord[i]].ric[j].size + barchetta.carico_pre <= SO_CAPACITY){
                        sh_mem_2.porti[ord[i]].ric[j].pre++;
                        merci_ric[j].pre++;
                        nmerci++;
                        barchetta.carico_pre += sh_mem_2.porti[ord[i]].ric[j].size;
                    }
                    printf("Nave %d: prenotati %d lotti di merce %d da consegnare al porto %d\n", barchetta.idn, merci_ric[j].pre, merci_ric[j].id, ord[i]);                 
                } else {
                    merci_ric[j].id = -1;
                }
            }
            
            if(flag){
                x = ord[i];
                break;
            }
        }
        if(!flag)
            printf("Nave %d: nessun porto utilizzabile! (scarico)\n", barchetta.idn);
        else
            printf("Totale carico: %f su %d\n", barchetta.carico_pre, SO_CAPACITY);
    }else
        printf("Nave %d piena!\n", barchetta.idn);
    UNLOCK
    return x;
}

void gennave()
{
    barchetta.coord.x = rand() % (int)(SO_LATO + 1);
    barchetta.coord.y = rand() % (int)(SO_LATO + 1);
    barchetta.carico = 0;
    barchetta.carico_pre = 0;
    printf("Creata nave n. %d in posizione %f, %f\n\n", barchetta.idn, barchetta.coord.x, barchetta.coord.y);
    sops.sem_num=1;
    sops.sem_op=1;
    semop(sem_id,&sops,1);
    sops.sem_num=2;
    sops.sem_op=-1;
    semop(sem_id,&sops,1);
}

void carico(){
    int i;
    int y;
    double work_time;
    int t = 0;
    double q; double nano;
    for(i = 0; i < MERCI_RIC_OFF; i++){
        if(merci_ric[i].id != -1){
            y = containsOff(barchetta.idp_part, merci_ric[i].id);
            if(y != -1){
                merci_ric[i].status = 1;
                LOCK
                sh_mem_2.porti[barchetta.idp_part].off[y].status = 1;
                t = merci_ric[i].pre;       
                while(t > 0){
                    barchetta.carico += merci_ric[i].size;
                    merci_ric[i].num++;
                    t--;
                }
                UNLOCK
                printf("Nave %d: caricati %d lotti di merce %d dal porto %d\n", barchetta.idn, merci_ric[i].num, merci_ric[i].id, barchetta.idp_part);
            }
        }
    }
    q=barchetta.carico/SO_LOADSPEED;
    work_time = q;
    nano=modf(q,&q);
    rem.tv_sec = q;
    rem.tv_nsec = nano*1e9;
    working = true;
    nanosleep(&rem, &rem);
    working = false;
    printf("Tempo di attesa: %f\n\n", work_time);
}

void scarico(){
    int i;
    int y;
    int t;
    double work_time;
    double q = 0; double nano=0;
    for(i = 0; i < MERCI_RIC_OFF; i++){
        if(merci_ric[i].id != -1 && merci_ric[i].status == 1){
            y = containsRic(barchetta.idp_dest, merci_ric[i].id);
            if(y != -1){
                merci_ric[i].status = 2;
                LOCK
                sh_mem_2.porti[barchetta.idp_dest].ric[y].status = 2;
                UNLOCK
                t = 0;
                while(merci_ric[i].pre > 0){
                    barchetta.carico -= merci_ric[i].size;
                    barchetta.carico_pre -= merci_ric[i].size;
                    q += merci_ric[i].size;
                    merci_ric[i].pre--;
                    t++;
                }
                merci_ric[i].size = 0;
                printf("Nave %d: consegnati %d lotti di merce %d al porto %d\n", barchetta.idn, t, merci_ric[i].id, barchetta.idp_dest);
                merci_ric[i].id = -1;
            }
        }
    }
    q=q/SO_LOADSPEED;
    work_time = q;
    nano=modf(q,&q);
    rem.tv_sec = q;
    rem.tv_nsec = nano*1e9;
    working = true;
    nanosleep(&rem, &rem);
    working = false;
    
    printf("Nave %d:\t ancora a bordo %f ton di merce\n\tancora prenotate %f ton di merce\n\ttempo di attesa: %f\n\n", barchetta.idn, barchetta.carico, barchetta.carico_pre, work_time);
}

int main (int argc, char * argv[]){
    int mem_id;
    int i;  
    double distance;
    double route_time; double nano;
    struct sigaction sa;
    size_t j;
    setvar();
    j=(sizeof(struct porto)+sizeof(struct merce)*2*MERCI_RIC_OFF_TOT)*SO_PORTI+(sizeof(struct merce))*SO_MERCI;
    sh_mem_2.porti=calloc(SO_PORTI,sizeof(struct porto)); 
    bzero(&sa,sizeof(sa));
    sa.sa_handler=SIG_IGN;
    ord=calloc(SO_PORTI,sizeof(int));
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler=handle_morte;
    sigaction(SIGINT, &sa, NULL);
    merci_ric=calloc(MERCI_RIC_OFF,sizeof(struct merce));  
    barchetta.idn= atoi(argv[1]);
    srand(getpid());
    /* Ottengo l'accesso a IPC obj */
    sem_id = semget(getppid()+1, NUM_SEMS, 0600 );
    msgN_id = msgget(getppid() + 3, 0600);
    msgP_id = msgget(getppid() + 4, 0600);
    bancid = semget(getppid()+2,SO_PORTI,0600|IPC_CREAT);
    mem_id=shmget(getppid(),j,0600);
    hlp=shmat(mem_id,NULL,0600);
    sh_mem.merci=(struct merce *) (hlp);
    hlp= (char *)(hlp + sizeof(struct merce) * SO_MERCI);
    sh_mem.porti = (struct porto *) ( hlp);
    hlp= (char *)(hlp + sizeof(struct porto) * SO_PORTI);
    for(i=0;i<SO_PORTI;i++)
    {
        sh_mem_2.porti[i].off=(struct merce*)hlp;
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF_TOT);
        sh_mem_2.porti[i].ric=(struct merce*) hlp;
        hlp=(char*)(hlp+sizeof(struct merce)*MERCI_RIC_OFF_TOT);
    }
    gennave();

    sa.sa_handler=handle_time;
    sigaction(SIGUSR1, &sa, NULL);
    sa.sa_handler=handle_storm;
    sigaction(SIGUSR2, &sa, NULL);
    sa.sa_handler=handle_swell;
    sigaction(SIGWINCH, &sa, NULL);

    do{
        if((barchetta.idp_dest = getdest()) == -1){
            printf("NAVE %d: nessun porto disponibile (scarico), aspetto nuovo giorno\n", barchetta.idn);
            rem.tv_sec = 1;
            rem.tv_nsec = 0;
            nanosleep(&rem, &rem);
            continue;
        }
        ordinaporti(sh_mem.porti[barchetta.idp_dest].coord);
        while(barchetta.carico_pre > 0){
            barchetta.idp_part = getpart();
            if(barchetta.idp_part != -1){
                LOCK
                printf("Nave %d: mi dirigo verso il porto %d\nDistanza: %f\n\n",barchetta.idn, barchetta.idp_part, distance = DISTANCE(sh_mem.porti[barchetta.idp_part].coord, barchetta.coord));
                UNLOCK
                msgN.id = barchetta.idn+1;
                msgN.pid = getpid(); 
                msgNf = true;
                if(msgsnd(msgN_id, &msgN, sizeof(msgN), 0) == -1){
                    TEST_ERROR;
                } else {
                    printf("Nave %d: inserita in coda %d con codice %ld\n", barchetta.idn, msgN_id, msgN.id);
                }

                route_time = distance / SO_SPEED;
                nano=modf(route_time,&route_time);
                rem.tv_sec = route_time;
                rem.tv_nsec = nano*1e9;
                
                rem.tv_sec = route_time;
                rem.tv_nsec = nano*1e9;
                travelling = true;
                printf("In viaggio per %ld sec e %ld nsec\n", rem.tv_sec, rem.tv_nsec);
                nanosleep(&rem, &rem);
                travelling = false;
                
                if(msgrcv(msgN_id, &msgN, sizeof(msgN), barchetta.idn+1, IPC_NOWAIT) == -1){
                    TEST_ERROR;
                } else {
                    printf("Nave %d: rimossa dalla coda\n",barchetta.idn);
                }
                msgNf = false;
                
                LOCK           
                barchetta.coord = sh_mem.porti[barchetta.idp_part].coord;
                UNLOCK
                
                printf("Nave %d: raggiunto porto %d, distante %f, dopo %f secondi\n", barchetta.idn, barchetta.idp_part, distance, (rem.tv_sec + rem.tv_nsec * 1e-9));
                LOCK_BAN (barchetta.idp_part);
                printf("Nave %d: inizio a caricare dal porto %d...\n", barchetta.idn, barchetta.idp_part);

                msgP.id = barchetta.idp_part+1;
                msgP.pid = getpid();
                msgPf1 = true;
                if(msgsnd(msgP_id, &msgP, sizeof(msgP), 0) == -1)
                    perror("Errore sndmsg carico");
                 else {
                    printf("Nave %d: aggiunto porto %d alla coda carico\n", barchetta.idn, barchetta.idp_part);
                }
                carico();
                if(msgrcv(msgP_id, &msgP, sizeof(msgP), barchetta.idp_part+1, IPC_NOWAIT) == -1){
                    perror("Errore msgrcv carico\n");
                } else {
                    printf("Nave %d: rimosso porto %d dalla coda\n",barchetta.idn, barchetta.idp_part);
                }
                msgPf1 = false;

                printf("Nave %d: finito di caricare\n\n", barchetta.idn);
                UNLOCK_BAN (barchetta.idp_part);

                LOCK
                printf("Nave %d: mi dirigo verso il porto %d\nDistanza: %f\n\n",barchetta.idn, barchetta.idp_dest, distance = DISTANCE(sh_mem.porti[barchetta.idp_dest].coord, barchetta.coord));
                UNLOCK

                msgN.id = barchetta.idn+1;
                msgN.pid = getpid();   
                msgNf = true;            
                if(msgsnd(msgN_id, &msgN, sizeof(msgN), 0) == -1){
                    TEST_ERROR
                } else {
                    printf("Nave %d: inserita in coda %d (ritorno) con codice %ld\n", barchetta.idn, msgN_id, msgN.id);
                }

                route_time = distance / SO_SPEED;
                nano=modf(route_time,&route_time);
                rem.tv_sec = route_time;
                rem.tv_nsec = nano*1e9;
                travelling = true;
                printf("In viaggio per %ld sec e %ld nsec\n", rem.tv_sec, rem.tv_nsec);
                nanosleep(&rem, &rem);
                travelling = false;
                if(msgrcv(msgN_id, &msgN, sizeof(msgN), barchetta.idn+1, IPC_NOWAIT) == -1){
                    perror("Errore msgrcv scarico\n");
                } else {                  
                    printf("Nave %d: rimossa dalla coda (ritorno)\n", barchetta.idn);
                }
                msgNf = false;

                LOCK
                barchetta.coord = sh_mem.porti[barchetta.idp_dest].coord;
                UNLOCK
                printf("Nave %d: raggiunto porto %d, distante %f, dopo %f secondi\n", barchetta.idn, barchetta.idp_dest, distance, (rem.tv_sec + rem.tv_nsec * 1e-9));
                LOCK_BAN (barchetta.idp_dest);
                printf("Nave %d: inizio a consegnare al porto %d...\n", barchetta.idn, barchetta.idp_dest);

                msgP.id = barchetta.idp_dest +1;
                msgP.pid = getpid();
                msgPf2 = true;
                if(msgsnd(msgP_id, &msgP, sizeof(msgP), 0) == -1){
                    perror("Errore msgsnd scarico");
                } else {
                    printf("Nave %d: aggiunto porto %d alla coda\n", barchetta.idn, barchetta.idp_dest);
                }
                scarico();
                if(msgrcv(msgP_id, &msgP, sizeof(msgN), barchetta.idp_dest+1, 0) == -1){
                    perror("Errore msgrcv scarico");
                } else {
                    printf("Nave %d: rimosso porto %d dalla coda\n",barchetta.idn, barchetta.idp_dest);
                }
                msgPf2 = false;

                printf("Nave %d: finito di consegnare\n\n", barchetta.idn);
                UNLOCK_BAN (barchetta.idp_dest);
                for(i = 0; i < MERCI_RIC_OFF; i++){
                    if(merci_ric[i].id!=-1)
                    break;
                }
                if(i==MERCI_RIC_OFF)
                    printf("Nave %d: merci prenotate esaurite\n",barchetta.idn);
                else{
                    printf("Ancora prenotate: ");
                    for(i = 0; i < MERCI_RIC_OFF; i++){
                        if(merci_ric[i].id!=-1)
                        printf("%d ton di %d + ", merci_ric[i].size * merci_ric[i].num, merci_ric[i].id);
                    }
                    printf("\n");
                }
            } else {
                printf("Nave %d: nessun porto offre le merci richieste dal porto %d\n", barchetta.idn, barchetta.idp_dest);
                barchetta.carico_pre = 0;
            }
        }
        printf("NAVE %d: finito il ciclo\n\n", barchetta.idn);
    }while(1);
    return 0;
}