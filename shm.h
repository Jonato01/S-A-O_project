
#ifndef SHM_H
#define SHM_H  
#define NUM_SEMS 1
#define SO_PORTI 6
#define SO_LATO 100.00 

/*0:    protezione shm*/
#define LOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = -1;            \
    sops.sem_flg = 0;              \
    semop(sem_id, &sops, 1);
#define UNLOCK                    \
    sops.sem_num = 0;            \
    sops.sem_op = 1;            \
    sops.sem_flg = 0;            \
    semop(sem_id, &sops, 1);

struct coordinates{
    double x;
    double y;
};

struct shared_data {
    int o;
    struct coordinates * all_ports;
};

struct merce {
    int id;
    int size;
    int vita;
};
#endif