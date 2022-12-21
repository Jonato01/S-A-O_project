
#ifndef SHM_H
#define SHM_H  
#define _GNU_SOURCE
#define NUM_SEMS 4
#define SO_PORTI 6
#define MAX_NUM_LOTTI 10
#define SO_LATO 100.00 
#define SO_NAVI 20
#define SO_GIORNI 30
#define NAVI_PATH_NAME "./nave"
#define PORTI_PATH_NAME "./porto"
#define SO_MERCI 12
#define MERCI_RIC_OFF 5
#define SO_SIZE 100 
#define SO_MIN_VITA 2 
#define S0_MAX_VITA 10
#define SO_SPEED 30
#define SO_CAPACITY 300
#define SO_BANCHINE 4
/*0:    protezione shm  1: creazione porti in ordine 2: controllare fine di ogni figlio*/
#define LOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = -1;            \
    semop(sem_id, &sops, 1);
#define UNLOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = 1;            \
    semop(sem_id, &sops, 1);



struct coordinates{
    double x;
    double y;
};
struct merce {
    int id;
    int size;
    int vita;
    int num;
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