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
#define SO_PORTI so_porti
#define SO_NAVI so_navi
#define SO_LATO so_lato
#define SO_GIORNI so_giorni
#define NAVI_PATH_NAME "./nave"
#define PORTI_PATH_NAME "./porto"
#define METEO_PATH_NAME "./meteo"
#define SO_MERCI so_merci
#define MERCI_RIC_OFF merci_ric_off
#define MERCI_RIC_OFF_TOT MERCI_RIC_OFF*SO_GIORNI
#define SO_SIZE so_size
#define SO_MIN_VITA so_min_vita /*vita minima merci*/ 
#define S0_MAX_VITA so_max_vita
#define SO_SPEED so_speed
#define SO_CAPACITY so_capacity
#define SO_BANCHINE so_banchine
#define SO_LOADSPEED so_loadspeed
#define SO_FILL so_fill
#define SO_STORM_DURATION so_storm_duration
#define SO_SWELL_DURATION so_swell_duration
#define SO_MAELSTROM so_maelstrom
int so_porti;int so_navi;int so_size; int so_maelstrom;
int so_merci;int so_capacity;int so_speed;
int so_banchine; int so_fill; int so_loadspeed;
double so_lato; int so_giorni; int merci_ric_off;
int so_min_vita; int so_max_vita; int so_storm_duration;
int so_swell_duration;
void setvar(){
    FILE *varf;
    size_t len=0; char*line;char*tr;
    varf=fopen("variabili.txt","rt");
    if(varf==NULL)
    {
        printf("file non aperto ");
        exit(1);
    }
    while(getline(&line,&len,varf)!=-1) {
        if((tr=strstr(line,"so_porti"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) { 
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else{
                so_porti=(atoi(tr));
            }
        } else if((tr=strstr(line,"so_navi"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 ||*tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_navi=atoi(tr);
        } else if((tr=strstr(line,"so_banchine"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
                tr++;
            if(*tr==10 ||*tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_banchine=atoi(tr);
        } else if((tr=strstr(line,"so_capacity"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
                tr++;
            if(*tr==10 ||*tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else    
                so_capacity=atoi(tr);
        } else if((tr=strstr(line,"so_giorni"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
                tr++;
            if(*tr==10 ||*tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_giorni=atoi(tr);
        } else if((tr=strstr(line,"so_lato"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) ||*tr==10 ||*tr==EOF))
                tr++;
            if(*tr==10 ||*tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_lato=atof(tr);
        } else if((tr=strstr(line,"so_loadspeed"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_loadspeed=atoi(tr);
        } else if((tr=strstr(line,"so_max_vita"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_max_vita=atoi(tr);
        } else if((tr=strstr(line,"so_merci"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_merci=atoi(tr);
        } else if((tr=strstr(line,"so_storm_duration"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_storm_duration=atoi(tr);
        } else if((tr=strstr(line,"so_swell_duration"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF) {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_swell_duration=atoi(tr);
        } else if((tr=strstr(line,"so_min_vita"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF){
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_min_vita=atoi(tr);
        }else if((tr=strstr(line,"so_maelstrom"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF){
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_maelstrom=atoi(tr);
        } else if((tr=strstr(line,"so_size"))!=NULL) {
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF){
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_size=atoi(tr);
        } else if((tr=strstr(line,"so_speed"))!=NULL){
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF){
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            } else
                so_speed=atoi(tr);
        } else if((tr=strstr(line,"merci_ric_off"))!=NULL){
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF)
            {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            }
            else{
                merci_ric_off=atoi(tr);
                if((so_merci/2)<merci_ric_off)
                {printf("var merci_ric_off troppo piccola verrà assegnata di default");
                 merci_ric_off=so_merci/2;
            }}
        } else if((tr=strstr(line,"so_fill"))!=NULL){
            while(!((*tr<=57 && *tr>=48) || *tr==10 || *tr==EOF))
                tr++;
            if(*tr==10 || *tr==EOF)
            {
                printf("err numero non trovato");
                exit(EXIT_FAILURE);
            }
            else
                so_fill=atoi(tr);
        }
         else 
            printf("trovata variabile inutilizzata");
    }
    
}

#endif