    
#ifndef SHM_H
#define SHM_H 
#include <stdbool.h>
#include "var.h"
#define NUM_SEMS 5
#define TEST_ERROR    if (errno!=EINTR && errno) {fprintf(stderr, \
					   "%s:%d: PID=%5d: Error %d (%s)\n",\
					   __FILE__,\
					   __LINE__,\
					   getpid(),\
					   errno,\
					   strerror(errno));}


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
/*0:    protezione shm  1: creazione porti in ordine 2: controllare fine di ogni figlio, 3: far partire navi e meteo insieme*/
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

struct my_msg_t {
    long int id; /* Ogni nave manda il proprio id quando in viaggio e lo ritira quando arrivata*/
    pid_t pid;
};

struct merce {
    int status; /*0: al porto, 1: su nave, 2: consegnato, 3: scaduta in porto*/
    int id;
    int size;
    int vita;
    int num;
    int pre;
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
    struct merce *ric;
    struct merce *off;
};

/*lunghezza shared memory*/
struct shared_data {
    struct merce * merci;
    struct porto * porti;  
};

#endif