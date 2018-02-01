// _cupspeller ; Projet MCS3
// OBIGAND Thomas
// PASQUET Rémi

#include "stream.h"

void ecrire(struct sockaddr_in svc, int s, int lenS);
void lire(struct sockaddr_in svc, int s, int lenS);

// fonction principale
int main(){
    struct sockaddr_in svc;
    int s, lenS, id_fork;
    char buff[MAX_BUFF];

    printf(":> creating socket..\n");
    CHECK(
        s = socket(
            AF_INET,
            SOCK_STREAM,
            0
        ),
        "!> error creating socket"
    );
    printf(">> socket created\n");

    svc.sin_family = AF_INET;
    svc.sin_port = htons(PORT_SRV);
    svc.sin_addr.s_addr = inet_addr(IP_SRV);
    memset(&svc.sin_zero,0,8);

    lenS = sizeof(svc);

    printf(":> connecting socket..\n");
    CHECK(
        connect(
            s,
            (struct sockaddr*)&svc,
            lenS
        ),
        "!> error connecting socket"
    );
    printf(">> socket connected\n");

    id_fork=fork();
	if(id_fork==0) {
		lire(svc, s, lenS);
	} else {
	  ecrire(svc, s, lenS);
	}

    close(1);
    return 0;

}

// fonction d'écriture sur la socket
void lire(struct sockaddr_in svc, int s, int lenS) {
    char buff[MAX_BUFF];

    while(1){

        lenS = sizeof(svc);
        memset(&buff[0], 0, sizeof(buff));

        CHECK(
            recv(
                s,
                buff,
                sizeof(buff),
                0
            ),
            "!> error receiving message"
        );
        fflush(stdout);
        printf("%s\n", buff);

    }
}

// fonction de lecture sur la socket
void ecrire(struct sockaddr_in svc, int s, int lenS) {
    char buff[MAX_BUFF];

    while(1) {

        fflush(stdin);
        fgets(buff, sizeof(buff), stdin);
        buff[MAX_BUFF] = '\0';

        CHECK(
            send(
                s,
                buff,
                strlen(buff)-1,
                0
            ),
            "!> error sending message"
        );
    }
}
