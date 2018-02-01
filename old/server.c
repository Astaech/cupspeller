// _cupspeller ; Projet MCS3
// OBIGAND Thomas
// PASQUET Rémi

#include "stream.h"

int sock_srv_id, new_sock_id, sock_len;
struct sockaddr_in sock_srv, sock_clt;
struct data_t player_list[MAX_CLT];
sem_t token;
fd_set read_fds;

void *playerThread(int id);
void *masterThread();
void initSocket();
void initThread();
void initData(int id);
void initSocketClient(int id);

// fonction principale
int main(int argc, char const *argv[]) {
  initSocket();
  initThread();

  close(sock_srv_id);
  return 0;
}

// thread gérant les connexions et les threads clients
void *masterThread() {
  int i, j, id, sd, max_sd, new_sock, list_length=771, run[MAX_WRD];
  char buff[MAX_BUFF], line[MAX_BUFF];
  struct word_t *list = malloc (list_length * sizeof (struct word_t));
  FILE *file;

  // récupération de la liste de mots depuis le fichier word_list.txt
  file = fopen(PATH, "r");

  while ((fgets(line, sizeof(line), file)) != NULL) {
    for (i = 0; i < sizeof(line); i++) {
      if(line[i] == '\n') {
        line[i] = '\0';
        break;
      }
    }
    strcpy(list[j].word, line);
    j++;
  }

  srand(time(NULL));

  // tirage aléatoire de 'MAX_WRD' dans la liste
  for(i=0; i<MAX_WRD; i++) {
    run[i] = rand() % list_length;
  }

  // boucle principale de connexion et déconnexion
  while(1) {
    j = 0;

    FD_ZERO(&read_fds);

    FD_SET(sock_srv_id, &read_fds);
    max_sd = sock_srv_id;

    for(i=0; i<MAX_CLT; i++) {
      sd = client(i);
      if(sd > 0) {
          FD_SET(sd, &read_fds);
          j++;
      }
      if(sd > max_sd) {
        max_sd = sd;
      }
    }

    if(j < MAX_CLT) {
      CHECK(
        select(
          max_sd+1,
          &read_fds,
          NULL,
          NULL,
          NULL
        ),
        "mast!> error with select\n"
      );

      // lorqu'une nouvelle connexion est établie, on crée un nouveau playerThread
      if(FD_ISSET(sock_srv_id, &read_fds)) {
        printf("mast:> listening socket..\n");
        CHECK(
          new_sock_id = accept(
            sock_srv_id,
            (struct sockaddr*) &sock_clt,
            &sock_len
          ),
          "mast!> error listening socket\n"
        );
        printf("mast>> new connection, fs %d, ip %s, port %d\n", new_sock_id, inet_ntoa(sock_clt.sin_addr), ntohs(sock_clt.sin_port));

        i=0;
        while(i < MAX_CLT) {
          if(client(i)==0) {
            client(i)=new_sock_id;
            break;
          }
          i++;
        }

        id=i;

        printf("mast:> creating mutex %d..\n", player(id).id);
        pthread_mutex_init(&mutex(id), NULL);

        pthread_mutex_lock(&mutex(id));

        player(id).id = id;
        strcpy(player(id).name, "astaech");

        strcpy(buff, "core> Bienvenue !");
        CHECK(
          send(
            client(id),
            buff,
            strlen(buff)+1,
            0
          ),
          "mast!> error sending message\n"
        );
        strcpy(buff,"\0");

        for(i=0; i<MAX_WRD; i++) {
          strcpy(progress(id).word_list[i].word, list[run[i]].word);
        }

        for(i=0; i<MAX_CLT; i++) {
          printf("mast>> player %d, socket : %d\n", i, client(i));
        }

        pthread_mutex_unlock(&mutex(id));

        printf("mast:> creating thread %d..\n", player(id).id);
        pthread_create(&(thread(id)), NULL, (void *)playerThread, (void *)id);
        pthread_detach(thread(id));

      }
    }

    // lorsqu'un client se déconnecte, on détruit le playerThread associé
    for(i=0; i<MAX_CLT; i++) {
      sd = client(i);

      if(FD_ISSET(sd, &read_fds)) {
        if(recv(sd, buff, sizeof (buff), 0) == 0) {
          getpeername(sd, (struct sockaddr*)&sock_clt, (socklen_t*)&sock_clt);
          printf("mast>> host disconnected, fs %d, ip %s, port %d\n", sd, inet_ntoa(sock_clt.sin_addr), ntohs(sock_clt.sin_port));
          close(sd);
          pthread_cancel(thread(i));
          pthread_mutex_destroy(&mutex(i));
          client(i) = 0;
        }
      }
    }
  }
  pthread_exit(0);
}

// thread gérant les interactions client / serveur
void *playerThread(int id) {
  int i;
  char msg_in[MAX_BUFF], msg_out[MAX_BUFF];
  struct timeval tval_before, tval_after, tval_result;
  initData(id);
  srand(time(NULL));

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

  printf("thd%d>> mutex created\nthd%d>> thread created\n", player(id).id, player(id).id);

  printf("thd%d>> client name is %s\n", id, player(id).name);
  printf("thd%d>> client socket is %d\n", id, client(id));

  gettimeofday(&tval_before, NULL);

  memset(&msg_in[0], 0, sizeof(msg_in));
  memset(&msg_out[0], 0, sizeof(msg_out));


  // boucle principale
  // envoi un mot de la liste de MAX_WRD choisi par masterThread
  // attend une réponse du client
  // si favorable, passe au mot suivant
  // calcul le temps total du client
  while(1) {
    if(strcmp(msg_in, progress(id).word_list[i].word) == 0)
      i++;
    if(i==MAX_WRD)
      break;

    pthread_mutex_lock(&mutex(id));
    sprintf(msg_out, ">%s", progress(id).word_list[i].word);
    printf("thd%d>> \"%s\" send\n", id, msg_out);
    CHECK(
      send(
        client(id),
        msg_out,
        strlen(msg_out)+1,
        0
      ),
      "thdx!> error sending message\n"
    );
    pthread_mutex_unlock(&mutex(id));

    memset(&msg_in[0], 0, sizeof(msg_in));
    memset(&msg_out[0], 0, sizeof(msg_out));

    CHECK(
      recv(
        client(id),
        msg_in,
        sizeof(msg_in),
        0
      ),
      "thdx!> error receiving message"
    );
    printf("thd%d:> \"%s\" received\n", id, msg_in);
  }

  gettimeofday(&tval_after, NULL);
  timersub(&tval_after, &tval_before, &tval_result);

  memset(&msg_in[0], 0, sizeof(msg_in));
  memset(&msg_out[0], 0, sizeof(msg_out));

  pthread_mutex_lock(&mutex(id));
  progress(id).time = tval_result;
  sprintf(msg_out, "core> Bien joué : %ld.%06lds", (long int)progress(id).time.tv_sec, (long int)progress(id).time.tv_usec);
  CHECK(
    send(
      client(id),
      msg_out,
      strlen(msg_out)+1,
      0
    ),
    "thdx!> error sending message\n"
  );
  strcpy(msg_out, "\0");
  pthread_mutex_unlock(&mutex(id));

  printf("thd%d>> thread destroyed\n", id);
  pthread_mutex_destroy(&mutex(id));
  pthread_exit(0);
}

// fonction d'initialisation du socket d'écoute du serveur
void initSocket() {
  printf("main:> creating socket..\n");
  CHECK(
      sock_srv_id = socket(
          AF_INET,
          SOCK_STREAM,
          0
      ),
      "main!> error creating socket\n"
  );
  printf("main>> socket created\n");

  sock_srv.sin_family = AF_INET;
  sock_srv.sin_port = htons(PORT_SRV);
  sock_srv.sin_addr.s_addr = inet_addr(IP_SRV);
  memset(&sock_srv.sin_zero,0,8);
  sock_len = sizeof(sock_srv);

  printf("main:> binding socket..\n");
  CHECK(
      bind(
          sock_srv_id,
          (struct sockaddr*)&sock_srv,
          sock_len
      ),
      "main!> error binding socket\n"
  );
  printf("main>> socket bound\n");

  printf("main:> socket listening..\n");
  CHECK(
      listen(
          sock_srv_id,
          1
      ),
      "main!> error listening socket\n"
  );
  printf("main>> socket ready to listen\n");
}

// fonction d'initialisation du thread de connexion
void initThread() {
  int i;
  pthread_t gameMaster;
  printf("main:> creating master mutex..\n");
  sem_init(&token, 0, 1);
  printf("main:> creating master thread..\n");
  pthread_create(&gameMaster, NULL, (void *)masterThread, (void *)NULL);
  pthread_join(gameMaster, NULL);
}

// fonction d'initialisation des données du joueur
void initData(int id) {
  pthread_mutex_lock(&mutex(id));
  player(id).rank = MAX_CLT;
  strcpy(player(id).name, "anonymous");
  player(id).points = 0;
  pthread_mutex_unlock(&mutex(id));
}
