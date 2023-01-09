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
#include "shm.h"

struct shared_data * sh_mem;

void tempesta(){
    int i;
    pid_t pid;
    while(1){
        pid = rand() % (SO_NAVI+1);
        LOCK
        if()
    }    

    
}

int main(){
    srand(getpid());
    sh_mem = shmat(mem_id, NULL, 0);

    while(1)
    shmdt ( sh_mem );
    exit(0);
}