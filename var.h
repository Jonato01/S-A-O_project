#ifndef VAR_H
#define VAR_H



 

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <strings.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#define SO_PORTI getporti()
#define SO_NAVI 5
int so_porti;int so_navi;int so_size;
int so_merci;int so_capacity;int so_speed;
int so_banchine; int so_fill; int so_loadspeed;
double so_lato; int so_giorni; int merci_ric_off;
int so_min_vita; int so_max_vita;
void setvar(){
    FILE *varf;
    size_t len=0; char*line;char*tr;
    varf=fopen("variabili.txt","rt");
    if(varf==NULL)
    {
        printf("file non aperto ");
        exit(1);
    }
    while(getline(&line,&len,varf)!=-1)
    {
        if((tr=strstr(line,"so_porti"))!=NULL)
        {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF)){
                printf("%c\n", *tr);
                tr++;
            }
            if(*tr==10 || *tr==EOF)
            { 
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            }
            else{
                so_porti=(atoi(tr));
                printf("\n so_porti: %d\n", so_porti);
            }
        }
        else


        if((tr=strstr(line,"so_navi"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
       {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_navi=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_size"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_size=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_merci"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
       {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_merci=atoi(tr);
        } 
        
        else
        if((tr=strstr(line,"so_capacity"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
        { printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_capacity=atoi(tr);
        }
        
        else
        if((tr=strstr(line,"so_speed"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_speed=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_banchine"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
            tr++;
        if(*tr==10 ||*tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_banchine=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_speed"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
        if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_speed=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_loadspeed"))!=NULL)
        {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
        if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_loadspeed=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_lato"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
        if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_lato=atof(tr);
        }
        else
        if((tr=strstr(line,"so_giorni"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
         if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_giorni=atoi(tr);
        }
        else
        if((tr=strstr(line,"merci_ric_off"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
          if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        merci_ric_off=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_min_vita"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
         if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        merci_ric_off=atoi(tr);
        }
        else
        if((tr=strstr(line,"so_max_vita"))!=NULL)
        {while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
            tr++;
        if(*tr==10 || *tr==EOF)
        {printf("err numero non trovato");
         exit(EXIT_FAILURE);
        }
        else
        so_max_vita=atoi(tr);
        }
        else printf("trovata variabile inutilizzata");
    }
    


}
int getporti(){
return so_porti;}

#endif