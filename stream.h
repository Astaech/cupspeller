// _cupspeller ; Projet MCS3
// OBIGAND Thomas
// PASQUET Rémi

#ifndef STREAM_H
#define STREAM_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>

#define CHECK(sts,msg) if((sts) == -1){perror(msg);exit(-1);}
#define PORT_SRV 5001
// adresse IP du serveur
#define IP_SRV "172.22.202.96"
// chemin absolu vers le fichier word_list.txt
#define PATH "/home/astaech/Bureau/_cupspeller/word_list.txt"
#define MAX_BUFF 30
#define MAX_CLT 4
#define MAX_WRD 10

#define player(i) player_list[i].player
#define progress(i) player_list[i].progress
#define client(i) player_list[i].sock_clt_id
#define thread(i) player_list[i].thread
#define mutex(i) player_list[i].mutex

// structure de mots
struct word_t {
	char word[MAX_BUFF];
};

// structure de données du joueur
struct player_t {
  int id, rank;
  char name[MAX_BUFF];
  float points;
};

// structure de données sur la progression du joueur
struct progress_t {
  int id;
  struct timeval time;
  struct word_t word_list[MAX_WRD];
};

/*
	structure de données principale, contenant :
	- les données d'un joueur
	- sa progression
	- la socket client
	- le thread associé
	- le mutex associé
*/
struct data_t {
  struct player_t player;
  struct progress_t progress;
	int sock_clt_id;
  pthread_t thread;
	pthread_mutex_t mutex;
};

#endif /* STREAM_H */
