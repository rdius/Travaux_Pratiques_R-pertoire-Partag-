/*
 * Chat Programme Groupe 1
 * Participants: DIALLO Aziz, KAFANDO Rodrique, KOUADIO Olivier		
 * IFI-PROMO 21
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//Definition par defaut du port, de l'addresse IP et de la taille max des messages
#define LISTENING_PORT 10000
#define IP_GROUPE "225.0.0.100"
#define MSG_TAILLE_MAX 256
//----------------------------
//For chat code, les commandes
#define GROUPE_CODE "gcd"
#define ONE_USER_CODE "onc"
#define USER_LIST_CODE "ulc"
#define KEEP_ALIVE_CODE "kac"
#define EXIT_CODE "exc"

#define SEPARATEUR_MSG ":"

#define STATUS_DECONNECTE "std"
#define STATUS_CONNECTE "stc"
//----------------------------
char nomUtilisateur[30] = "group1";
//liste d'utilisateur
char listUser[30][30];
int nombre = 0;

pthread_t *thread_id;

struct sockaddr_in addr1;
char msgSend[MSG_TAILLE_MAX] = "";
char msgIn[MSG_TAILLE_MAX] = "";
char msgAlive[MSG_TAILLE_MAX] = "";

struct sockaddr_in addr;
int fd, nbytes, addrlen;
int fd1;
struct ip_mreq mreq;
char msgbuf[MSG_TAILLE_MAX];
u_int yes = 1; 
unsigned char ttl = 32;

//Prototypage des fonctions
void *receivMsg(void * arg);
void *sendMsg(void * arg);
void *keepAlive(void * arg);
void *setZero(void * arg);
	//Fonction de traitement de messages
int msgSendProcess(char *msg);
void msgRcvProcess(char *msg);
void strnCut(char *strDst, char *strSrc, int n);
void strcCut(char *strDst, char *strSrc, char c);
void getnomUtilisateur(char *strDst, char *strSrc, char c);

	//Enregistrement d'un utilisateur
void saveUser(char *user);
	//Affichage de la liste des utilisateurs
void printUser();
	//Envoie de messages
void sendMsgsg();
	//Affichage du menu utiilisateur
void printGuide();

int main(int argc, char *argv[]) {
	strcpy(nomUtilisateur, argv[1]);
	//Creation d'un port UDP native
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

	//Autorisation de plsuieurs sockets à utiliser le meme port
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("La réutilisation de l'address à échouée");
		return 1;
	}
	
	// Definition d'un TTL (Time-To-Live)
	setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

	//Definition de l'address de destination
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY ); 
	addr.sin_port = htons(LISTENING_PORT);

	//Recuperation de l'address de destination
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	//Utilisation de setsockopt() pour autoriser la jonction à un groupe de diffusion multicast
	mreq.imr_multiaddr.s_addr = inet_addr(IP_GROUPE);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY );
	if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq))
			< 0) {
		perror("setsockopt");
		return 1;
	}
	//-----------------------------------------------------------------------------
	//struct sockaddr_in addr1;
	//Creation d'un socket UDP ordinaire
	if ((fd1 = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return 1;
	}
	////Definition de l'address de destination
	memset(&addr1, 0, sizeof(addr1));
	addr1.sin_family = AF_INET;
	addr1.sin_addr.s_addr = inet_addr(IP_GROUPE);
	addr1.sin_port = htons(LISTENING_PORT);
	//-----------------------------------------------------------------------------
	//Definition d'un TTL (Time-To-Live)
	setsockopt(fd1, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
//------------------------------------------------------------------------------------
	//Affichage de la liste des commandes
	printGuide();	
	// Creation d'un file de chat 
	thread_id = (pthread_t *) malloc(3 * sizeof(pthread_t));

	// Chat textuel
	pthread_create(&thread_id[0], NULL, receivMsg, NULL);
	pthread_create(&thread_id[1], NULL, sendMsg, NULL);
	pthread_create(&thread_id[2], NULL, keepAlive, NULL);
	pthread_create(&thread_id[3], NULL, setZero, NULL);

	pthread_join(thread_id[0], NULL);
	pthread_join(thread_id[1], NULL);
	pthread_join(thread_id[2], NULL);
	pthread_join(thread_id[3], NULL);
//------------------------------------------------------------------------------------
}

//Recepteur Multicast 
void *receivMsg(void * arg) {
	for (;;) {

		if ((nbytes = recv(fd, msgbuf, MSG_TAILLE_MAX, 0)) < 0) {
			perror("recvfrom");
		}
		msgRcvProcess(msgbuf);
	}
	pthread_exit(0);
}

//Source Multicast
void *sendMsg(void * arg) {
	for (;;) {
		fgets(msgIn, sizeof(msgIn), stdin);
		int a = msgSendProcess(msgIn);
		if (a == 0) {
			if (sendto(fd1, msgSend, MSG_TAILLE_MAX, 0, (struct sockaddr *) &addr1,
					sizeof(addr1)) < 0) {
				perror("sendto");
			}
		}
		else if(a == 2){
			if (sendto(fd1, msgSend, MSG_TAILLE_MAX, 0, (struct sockaddr *) &addr1,
					sizeof(addr1)) < 0) {
				perror("sendto");
			}
			exit(0);
		}
	}
	pthread_exit(0);
}
//Envoie de message
void sendMsgsg() {
	if (sendto(fd1, msgSend, MSG_TAILLE_MAX, 0, (struct sockaddr *) &addr1,
					sizeof(addr1)) < 0) {
				perror("sendto");
			}
	
}
//Source  Multicast
void *keepAlive(void * arg) {
	for (;;) {
		strcpy(msgAlive, "");
		strcpy(msgAlive, KEEP_ALIVE_CODE);
		strcat(msgAlive, nomUtilisateur);
		if (sendto(fd1, msgAlive, MSG_TAILLE_MAX, 0, (struct sockaddr *) &addr1,
				sizeof(addr1)) < 0) {
			perror("sendto");
		}
		sleep(5);
		
	}
	pthread_exit(0);
}
//Mise à jour de la connection
void *setZero(void * arg){
	for (;;) {
		sleep(31);
		nombre=0;
	}
}

//Traitement des commandes
int msgSendProcess(char *msg) {
	char tmp[MSG_TAILLE_MAX] = "";
	char rcveur[MSG_TAILLE_MAX] = "";
	if (strncmp(msg, "2", 1) == 0) {
		strnCut(tmp, msg, 1);
		strcpy(msg, tmp);
		//Preparation des message pour envoie
		strcpy(msgSend, "");
		strcpy(msgSend, GROUPE_CODE);
		strcat(msgSend, nomUtilisateur);
		strcat(msgSend, ":");
		strcat(msgSend, msg);
		return 0;
	} else if (strncmp(msg, "3:", 2) == 0) {
		//Chat avec une personne
		strnCut(tmp, msg, 2);
		strcpy(msg, tmp);
		//choix du destinataire 
		strcCut(rcveur, msg, ':');
		//Preparation des message pour envoie
		strcpy(msgSend, "");
		strcpy(msgSend, ONE_USER_CODE);
		strcat(msgSend, nomUtilisateur);
		strcat(msgSend, ":");
		strcat(msgSend, msg);
		return 0;
	} else if (strncmp(msg, "4", 1) == 0) {
		//Former le message pour envoyer
		strcpy(msgSend, "");
		strcpy(msgSend, USER_LIST_CODE);
		strcat(msgSend, nomUtilisateur);
		return 0;
	} else if (strncmp(msg, "1", 1) == 0) {
		printUser();
		return 1;
	} else if (strncmp(msg, "5", 1) == 0) {
		//Preparation des message pour envoie
		strcpy(msgSend, "");
		strcpy(msgSend, KEEP_ALIVE_CODE);
		strcat(msgSend, nomUtilisateur);
		return 0;
	} else if (strncmp(msg, "6", 1) == 0) {
		//Preparation des message pour envoie
		strcpy(msgSend, "");
		strcpy(msgSend, EXIT_CODE);
		strcat(msgSend, nomUtilisateur);
		return 2;
	} else {
		//chat avec tous les membres du groupe 
		strcpy(msgSend, "");
		strcpy(msgSend, GROUPE_CODE);
		strcat(msgSend, nomUtilisateur);
		strcat(msgSend, ":");
		strcat(msgSend, msg);
		return 0;
	}
}

//Reception des messages
void msgRcvProcess(char *msg) {
	char tmp[MSG_TAILLE_MAX] = "";
	char tmp2[MSG_TAILLE_MAX] = "";
	char tmp3[MSG_TAILLE_MAX] = "";
	char sender[MSG_TAILLE_MAX] = "";
	char rcveur[MSG_TAILLE_MAX] = "";

	if (strncmp(msg, GROUPE_CODE, 3) == 0) {
		strnCut(tmp, msg, 3);
		printf("Group:%s\n", tmp);
	} else if (strncmp(msg, ONE_USER_CODE, 3) == 0) {
		strnCut(tmp, msg, 3);
		strcCut(sender, tmp, ':');
		getnomUtilisateur(tmp2, tmp, ':');
		strcCut(rcveur, tmp2, ':');
		getnomUtilisateur(tmp3, tmp2, ':');
		if (strcmp(nomUtilisateur, sender) == 0 || strcmp(nomUtilisateur, rcveur) == 0) {
			printf("%s:", sender);
			printf("to %s:", rcveur);
			printf("%s\n", tmp3);
		}
	} else if (strncmp(msg, USER_LIST_CODE, 3) == 0) {
		//Envoie d'un alive au cas ou un nouveau utilisateur se connecte
		strcpy(msgSend, "");
		strcpy(msgSend, KEEP_ALIVE_CODE);
		strcat(msgSend, nomUtilisateur);
		sendMsgsg();
	} else if (strncmp(msg, KEEP_ALIVE_CODE, 3) == 0) {
		strnCut(tmp, msg, 3);
		saveUser(tmp);
	} else if (strncmp(msg, EXIT_CODE, 3) == 0) {
		strnCut(tmp, msg, 3);
		strcpy(sender, tmp);
	}
}

//Cut string depuis un charatere n
void strnCut(char *strDst, char *strSrc, int n) {
	for (int i = n; i < strlen(strSrc); i++) {
		strDst[i - n] = strSrc[i];
	}
	strDst[strlen(strSrc) - n + 1] = '\0';
}
//Cut string avec une charatere
void strcCut(char *strDst, char *strSrc, char c) {
	char * pch = strchr(strSrc, c);
	if (pch != NULL) {
		strncpy(strDst, strSrc, pch - strSrc);
		
	}
}
//Recuperation du nom d'un utiilisateur
void getnomUtilisateur(char *strDst, char *strSrc, char c) {
	char * pch = strchr(strSrc, c);
	if (pch != NULL) {
		strcpy(strDst, 1 + pch);
		
	}
}
//Enregistrement d'un utilisateur
void saveUser(char *user) {
if (strcmp(nomUtilisateur, user) != 0){
	if (nombre == 0) {
		strcpy(listUser[nombre], user);
		nombre++;
	} else {
		int tmp = -1;
		for (int i = 0; i < nombre; i++) {
			if ((strcmp(listUser[i], user) == 0)) {
				tmp = i;
			}
		}
		if (tmp == -1) {
			strcpy(listUser[nombre], user);
			nombre++;
		}
	}
}
}

//Afficher la liste des utiilisateurs présents dans le réseau
void printUser() {
	printf("Liste des utilisateurs connectés:\n");
	for (int i = 0; i < nombre; i++) {
		printf("%d:", i+1 );
		puts(listUser[i]);
	}
//Notifier lorsqu'il n'y a qu'un seul utlisateur dans le groupe
	if (nombre==0) {
	printf("Vous etes le seul dans le groupe pour le moment...\n");

}
}
//Menu utilisateur
void printGuide(){
	printf("***CETTE APPLICATION GÈRE UN CHAT DANS UN RÉSEAU LOCAL***\n\n");
	printf("Veuillez saisir l'une des commandes suivantes et appuyez sur ENTRÉE \n\n");

	printf("1 => Pour afficher la liste des membres dans le  groupe\n\n");
	printf("2 => Pour envoyer un message à tous les membres du groupe\n\n");
	printf("3:user_name:message_à_envoyer => Pour envoyer un message à une seule personneà partir de son user_name\n\n");
	printf("4 => Pour mettre à jour la liste des utilisateurs\n\n");
	printf("5 => Pour signaler sa présence dans le groupe\n\n");
	printf("6 => Pour quitter l'application\n\n");

	printf("*************************************************\n");
}

