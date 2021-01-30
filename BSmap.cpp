#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "BSmap.h"



int main(int argv, char **argc)
{
	
	int fd,i;
	char input;
	char commande[50];
	string s1;
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
			getline(std::cin, s1); // pour récupérer même les espaces
			memcpy(commande,s1.c_str(),s1.size());
			commande[s1.size()] = '\0';
			printf("on a reçu %s de la taille %d\n",commande, s1.size());
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
			sizeRead=0;
			while(read(fd,&input,1)>0)
			{
				strRead[sizeRead++]=input;
				//printf("%c",input);
			}
			decode(strRead,sizeRead);
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
// t ou T : consigne de cible (target), TP pour polaire, TR pour rotation
// a ou A : consigne d'actionneur
// s ou S : test, on attend un ack simple
int encode(char *request)
{
	char tmpchar[3];
	int val1, val2;
	short vals1, vals2;
	printf("Encodage de : %s\n",request);
	switch (request[0]) // reçu depuis l'interface
	{
		default :
			printf("C'est quoi ça??\n");
			return (-1);
		case 's':
		case 'S':
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
		case 't':
		case 'T':
			strWrite[0] = ID_ORDER;
			strWrite[2] = VersionRobot;
			if (request[1] == 'P' || request[1] == 'p') // polar
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d %d",tmpchar, &val1, &val2);
				printf("demande de polar vers (%d : %d)\n",val1, val2);
				vals1 = (short)val1;vals2=(short)val2;
				strWrite[3] = 'P'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = (char) ((int)vals2 & 0x00FF);
				strWrite[7] = (char) (((int)vals2)>>8 & 0x00FF);
				strWrite[8] = 0; //angle
				strWrite[9] = 0; //angle
				strWrite[10] = checkSum();
				strWrite[1] = 7;
				sizeWrite = 11;
			}
			if (request[1] == 'R' || request[1] == 'r') // rotation
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d",tmpchar, &val1);
				printf("demande de rotation vers un angle de %d° \n",val1);
				vals1 = (short)val1;
				strWrite[3] = 'R'; //polar
				strWrite[4] = 0; //X
				strWrite[5] = 0; //X
				strWrite[6] = 0; //Y
				strWrite[7] = 0; //Y
				strWrite[8] = (char) ((int)vals1 & 0x00FF);
				strWrite[9] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[10] = checkSum();
				strWrite[1] = 7;
				sizeWrite = 11;
			}
			break;
		// les autres sont à faire
	}
	return 0;
}
int decode(char *trame,int t)
{
	printf("début du décodage de");
	for (int j=0;j<sizeRead;j++)
		printf("%d ",trame[j]);
	printf("\n");
	switch (trame[0]) //l'identifiant
	{
		case ID_POSITION :
			//printf("demande de position t= %d\n",t);
			if (t < 13) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curPos.posX = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curPos.posY = (signed short)(trame[5])+((int)(trame[6]) << 8);
			curPos.posAlpha = (signed short)(trame[7])+((int)(trame[8]) << 8);
			printf("la position est %d %d %d\n",curPos.posX, curPos.posY, curPos.posAlpha);
			break;
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

