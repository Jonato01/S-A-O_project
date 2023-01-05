
#ifndef SHM_H
#define SHM_H 
#include <stdbool.h>
#define NUM_SEMS 5
#define SO_PORTI 10
#define SO_LATO 100.00 
#define SO_NAVI 1
#define SO_GIORNI 10
#define NAVI_PATH_NAME "./nave"
#define PORTI_PATH_NAME "./porto"
#define SO_MERCI 12
#define MERCI_RIC_OFF 6
#define SO_SIZE 100 
#define SO_MIN_VITA 2 /*vita minima merci*/ 
#define S0_MAX_VITA 5
#define SO_SPEED 50
#define SO_CAPACITY 1000
#define SO_BANCHINE 4
#define SO_LOADSPEED 200
#define SO_FILL SO_PORTI*MERCI_RIC_OFF*(SO_SIZE/2+1)*3

/*blocca la banchina all'attracco*/
#define LOCK_BAN(ID);   \
sops.sem_num =ID;       \
sops.sem_op=-1;         \
semop(bancid,&sops, 1);
 /*sblocca la banchina dopo che la nave lascia il porto*/
#define UNLOCK_BAN(ID);     \
sops.sem_num =ID;           \
sops.sem_op=1;              \
semop(bancid,&sops, 1);

#define DISTANCE(a, b) sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2))
/*0:    protezione shm  1: creazione porti in ordine 2: controllare fine di ogni figlio*/
#define LOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = -1;            \
    semop(sem_id,&sops, 1);
#define UNLOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = 1;            \
    semop(sem_id,&sops, 1);

struct coordinates{
    double x;
    double y;
};

struct merce {
    int status; /*0: al porto, 1: su nave, 2: consegnato, 3: scaduta*/
    int id;
    int size;
    int vita;
    int num;
    int pre;
    pid_t pid_navi[SO_NAVI];
};
struct nave
{
    int idn;
    struct coordinates coord;
    int idp_dest;/* final destination fox only dove consegnamo */
    int idp_part;/* dove carichiamo */
    double carico;
    double carico_pre;
};

struct porto {
    int idp;
    
    struct coordinates coord;
    struct merce ric[MERCI_RIC_OFF];
    struct merce off[MERCI_RIC_OFF];
};


struct shared_data {
    struct merce merci[SO_MERCI];
    struct porto porti[SO_PORTI];    
};



#endif