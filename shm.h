
#ifndef SHM_H
#define SHM_H  
#define NUM_SEMS 1
//0:    protezione shm
//1:    aspetta che termino tutti i processi
#define LOCK                    \
    sops.sem_num = 1;            \
    sops.sem_op = -1;            \
    sops.sem_flg = 0;            \
    semop(sem_id, &sops, 1);
#define UNLOCK                    \
    sops.sem_num = 1;            \
    sops.sem_op = 1;            \
    sops.sem_flg = 0;            \
    semop(sem_id, &sops, 1);
struct shared_data {
	/* index where next write will happen */
	unsigned long cur_idx;
	
	double map_size;

	int j;  
};
#endif