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
	// pour l'instant on est en ligne de commande. On demande donc
	// une trame ou on donne un ordre par l'entrée strandard
	i=0;
	while(1)
	{
		do 
		{
			getline(std::cin, s1); // pour récupérer même les espaces
			memcpy(commande,s1.c_str(),s1.size());
			commande[s1.size()] = '\0';
			//printf("on a reçu %s de la taille %d\n",commande, s1.size());
		}
		// encodage de la ligne reçue
		while(encode(commande)!=0);
		// puis écriture dans le pipe
		fd=open(fifowrite,O_WRONLY);
		if (fd != -1)
		{
			write(fd,strWrite, sizeWrite);
		}
		else
			printf("pipe HS\n");
		close (fd);
		//printf("Ecriture faite\n");
		
		//on va lire la réponse dans l'autre pipe
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
		usleep(500000);
	}
	return 0;

}
// types de commande, pour l'instant
// p ou P : demande de trame position
// r ou R : demande de trame robot
// v ou V : demande de trame asservissement
// a ou A : demande de trame actionneurs
// s ou s : demande de trame sensors
// t ou T : consigne de cible (target), TP pour polaire, TR pour rotation
// d ou D : consigne d'actionneur
// x ou X : test, on attend un ack simple
int encode(char *request)
{
	char tmpchar[3];
	int val1, val2,j;
	short vals1, vals2;
	
	memset(strWrite,0,100);
	printf("Encodage de : %s\n",request);
	switch (request[0]) // reçu depuis l'interface
	{
		default :
			printf("C'est quoi ça??\n");
			return (-1);
		case 'h':
		case 'H':
			printf("P : demande trame Position\n");
			printf("V : demande trame Asservissement\n");
			printf("R : demande trame Robot\n"); 
			printf("A : demande trame Actionneur\n");
			printf("S : demande trame Sensors\n");
			printf("M : demande trame motors\n");
			printf("TP valx valy : cible avant\n");
			printf("TI valx valy : cible arrière\n");
			printf("TR angle : cible en rotation\n");
			printf("TM vitDroite vitGauche : mode manuel en vitesse\n");
			printf("CG calib : renvoie la valeur de la calibration\n");
			printf("CS calib valeur : impose la valeur à la calibration\n");
			printf("CW : écriture de la config dans le fichier \n");
			printf("H : help\n");
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
		case 'v':
		case 'V': // demande d'asservissement
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ASSERV;
			strWrite[4] = checkSum();
			break;
		case 'r':
		case 'R':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ROBOT;
			strWrite[4] = checkSum();
			break;
		case 'a':
		case 'A':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_ACTION;
			strWrite[4] = checkSum();
			break;
		case 's':
		case 'S':
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_SENSORS;
			strWrite[4] = checkSum();
			break;
		case 'm':
		case 'M': // demande de motors
			sizeWrite = 5;
			strWrite[0] = ID_INFO;
			strWrite[1] = 1;
			strWrite[2] = VersionRobot;
			strWrite[3] = ID_MOTORS;
			strWrite[4] = checkSum();
			break;
		case 't':
		case 'T':
			strWrite[0] = ID_ORDERMOVE;
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
			else if (request[1] == 'M' || request[1] == 'm') // manual speed
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d %d",tmpchar, &val1, &val2);
				printf("demande de manual (d= %d : g= %d)\n",val1, val2);
				vals1 = (short)val1;vals2=(short)val2;
				strWrite[3] = 'M'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = (char) ((int)vals2 & 0x00FF);
				strWrite[7] = (char) (((int)vals2)>>8 & 0x00FF);
				strWrite[8] = checkSum();
				strWrite[1] = 5;
				sizeWrite = 9;
			}
			else if (request[1] == 'R' || request[1] == 'r') // rotation
			{
				//strcpy(request,"TP 36 32");
				sscanf(request,"%s %d",tmpchar, &val1);
				printf("demande de rotation vers un angle de %d° \n",val1);
				vals1 = (short)val1;
				strWrite[3] = 'R'; //polar
				strWrite[4] = (char) ((int)vals1 & 0x00FF);
				strWrite[5] = (char) (((int)vals1)>>8 & 0x00FF);
				strWrite[6] = checkSum();
				strWrite[1] = 3;
				sizeWrite = 7;
			}
			else if (request[1] == 'S' || request[1] == 's') // stop
			{
				//strcpy(request,"TP 36 32");
				printf("demande d'arrêt \n");
				strWrite[3] = 'S'; //polar
				strWrite[4] = checkSum();
				strWrite[1] = 1;
				sizeWrite = 5;
			}
			else
			{
				printf("Demande de déplacement inconnue. Tapez H pour l'aide\n");
				return -1;
			}
			break;
		case 'c':
		case 'C': // demande de calibration
			//strcpy(calname,request);
			strWrite[0] = ID_PARAM;
			strWrite[1] = strlen(request);
			strWrite[2] = VersionRobot;
			for (j=0;j<strlen(request);j++)
				strWrite[j+3] = toupper(request[j]);
			strWrite[j+4] = checkSum();
			sizeWrite = strWrite[1]+4;
			
			for (j=0;j<max(0,(int)(strlen(request)))-3;j++)
				calname[j]=toupper(request[j+3]);
			calname[j+1]='\0';
			//printf("1");
			break;
		// les autres sont à faire
	}
	return 0;
}
int decode(char *trame,int t)
{
	char calstr[20];
	printf("début du décodage de ");
	for (int j=0;j<sizeRead;j++)
		printf("%x ",trame[j]);
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
			curPos.spdFor = (signed short)(trame[9])+((int)(trame[10]) << 8);
			curPos.spdRot = (signed short)(trame[11])+((int)(trame[12]) << 8);
			printf("la position est %d %d %d\n",curPos.posX, curPos.posY, curPos.posAlpha);
			printf("la vitesse est %d mm/s %d°/s\n",curPos.spdFor, curPos.spdRot);
			break; 
		case ID_ASSERV :
			if (t < 13) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curAss.type = trame[3];
			curAss.tarX = (signed short)(trame[4])+((int)(trame[5]) << 8);
			curAss.tarY = (signed short)(trame[6])+((int)(trame[7]) << 8);
			curAss.tarAlpha = (signed short)(trame[8])+((int)(trame[9]) << 8);
			curAss.spdFor = (signed short)(trame[10])+((int)(trame[11]) << 8);
			curAss.spdRot = (signed short)(trame[12])+((int)(trame[13]) << 8);
			curAss.conv = trame[14];
			curAss.block = trame[15];
			printf("type : %d converge = %d\n",curAss.type,curAss.conv);
			printf("la cible est %d %d %d\n",curAss.tarX, curAss.tarY, curAss.tarAlpha);
			printf("les vitesses for %d et rot %d\n",curAss.spdFor, curAss.spdRot);
			break;
		case ID_ROBOT :
			if (t < 8) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curRob.state = trame[3];
			curRob.color = trame[4];
			curRob.count = (int)(trame[5])+((int)(trame[6]) << 8); // en top 20ms
			curRob.type = trame[7];
			curRob.score = (uint8_t) (trame[8]);
			printf("count %.2f :score = %d\n",(float)(curRob.count)/50.0,curRob.score);
			break;
		case ID_ACTION :
			if (t < 5) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curAct.state1 = trame[3];
			curAct.state2 = trame[4];
			//printf("count : %d score = %d\n",curRob.count,curRob.score);
			break;
		case ID_MOTORS :
			if (t < 5) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curMot.spdLeft = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curMot.spdRight = (signed short)(trame[5])+((int)(trame[6]) << 8);
			curMot.powerLeft = trame[7];
			curMot.powerRight = trame[8];
			printf("vitesses moteurs : D %d G %d\n",curMot.spdRight, curMot.spdLeft);
			break;
		case ID_SENSORS :
			if (t < 3) // on n'a pas toute la trame
				return -1;
			// on va vérifier la version du robot, la longueur...
			//puis les data
			curSen.angleLeft = (signed short)(trame[3])+((int)(trame[4]) << 8);
			curSen.angleRight = (signed short)(trame[5])+((int)(trame[6]) << 8);
			printf("codeurs : D %d G %d\n",curSen.angleRight, curSen.angleLeft);
			break;
		case ID_CALGET :
			for (int i=0;i<strlen(trame)-5;i++)
				calstr[i]=trame[i+3];
			printf("%s = %s\n",calname,calstr);
			break;
		case ID_ACK:
			printf("Ack reçu\n");
			break;
		case ID_NACK:
			printf("NACK %d\n",trame[4]);
			break;
		default :
			printf("trame inconnue %d\n",trame[0]);
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

