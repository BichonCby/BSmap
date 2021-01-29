#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "BSmap.h"

int encode(char *request);
int decode(char *trame);
char checkSum();
char strWrite[100];
int sizeWrite;

int main(int argv, char **argc)
{
	
	int fd,i;
	char input;
	char commande[50];
	const char * fiforead = "/tmp/RobBSmap"; // du robot vers l'outil
	const char * fifowrite = "/tmp/BSmapRob"; // de l'outil vers le robot
	// création des fifo, au cas où elles n'existeraient pas
	if (mkfifo(fiforead,0777) != 0 || mkfifo(fifowrite,0777) != 0)
	{
		printf("imposible de créer le pipe \n");
		//return;
	}
	printf("pipe OK\n");
	// on va lire et attendre de recevoir quelque chose
	// début de la machine à état pour la com remote
	// pour l'instant on boucle en lecture écriture
	i=0;
	while(1)
	{
		do 
		{
			scanf("%s",commande);
			printf("aon a reçu %s\n",commande);
		}
		while(encode(commande)!=0);
		fd=open(fifowrite,O_WRONLY);
		if (fd != -1)
		{
			write(fd,strWrite, sizeWrite);
		}
		else
			printf("pipe HS\n");
		close (fd);
		//printf("Ecriture faite\n");
		//on va lire la réponse
		fd=open(fiforead,O_RDONLY);
		if (fd != -1) // la lecture s'est bien passée
		{
			printf("Reçu par le lecteur :\n");
			// traitement
			while(read(fd,&input,1)>0)
			{
				
				printf("%c",input);
			}
			printf("La lecture se termine!\n");
		}
		else
			printf("pipe HS\n"); // oups!
		close (fd);
		usleep(5000000);
	}
	return 0;

}
// types de commande, pour l'instant
// p ou P : demande de trame position
// c ou C : consigne de cible
// a ou A : consigne d'actionneur
// t ou T : test, on attend un ack simple
int encode(char *request)
{
	printf("Encodage de : %s\n",request);
	switch (request[0]) // reçu depuis l'interface
	{
		default :
			printf("C'est quoi ça??\n");
			return (-1);
		case 't':
		case 'T':
			strcpy(strWrite,"coucou");
			sizeWrite = 6;
			break;
		case 'p':
		case 'P': // demande de position
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_POSITION;
			strWrite[4] = checkSum();
			break;
		// les autres sont à faire
	}
	return 0;
}
// calcule le checksum, à savoir la somme de tous les octets, casté en char
char checkSum()
{
	int cs=0;
	for (int i=0;i<strWrite[1]+3;i++)
		cs+=(int)strWrite[i];
	return (char)cs;
}
// fonction de décodage octet par octet de la trame reçue. Pour l'instant on écrit
/*int decodeByte(char b)
{
	printf("%c",b);
	return 0;
}

int Remote::encodeFrame(char id)
{
	// l'identifiant est la question
	// si c'est un ordre, on va répondre OK avec le numéro de l'ordre
	// si c'est un requete de trame, on renvoie la trame
	float val1, val2, val3;
	switch (id)
	{
		case ID_ACK :
			strWrite[0] = ID_ACK; // identifiant
			strWrite[1] = 2;// taille utile hors version
			strWrite[2] = Rob.getVersion();
			strWrite[3] = strRead[3]; // l'odre donné
			
			strWrite[4] = ACK_OK; // pour l'instant tout est OK
			sizeWrite = 5;
			break;
		case ID_POSITION :
			strWrite[0] = ID_POSITION;
			strWrite[1] = 10;// taille utile
			strWrite[2] = Rob.getVersion();
			Pos.getPosition(&val1,&val2,&val3);
			strWrite[3] = (char) ((int)val1 & 0x00FF); // pos X poids faible
			strWrite[4] = (char) (((int)val1)>>8 & 0x00FF); // pos X poids fort
			strWrite[5] = (char) ((int)val2 & 0x00FF); // pos Y poids faible
			strWrite[6] = (char) (((int)val2)>>8 & 0x00FF); // pos Y poids fort
			strWrite[7] = (char) ((int)val3 & 0x00FF); // pos A poids faible
			strWrite[8] = (char) (((int)val3)>>8 & 0x00FF); // pos A poids fort
			// ...
			sizeWrite = 13;
			break;
		default :
			break;
	}
	return 0;
	
}*/
