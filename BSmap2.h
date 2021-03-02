//#include <iostream> // avec g++
//using namespace std;
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wiringPi.h>
//#define uint8_t char
//#define bool char
//#define true 1
//#define false 0

#define max(a,b) (a>b?a:b)
#define toupper(g) g
#define sleeps(t) usleep(t*1000000)

#define STREMOTE_WAITINI 1
#define STREMOTE_WAIT_RECEIVE 2
#define STREMOTE_WAIT_SEND 3

// les identifiants de trame
#define ID_INFO 		0x01
#define ID_ORDERMOVE 	0x02
#define ID_ORDERACT 	0x03
//#define ID_CALSET 		0x0A
#define ID_CALGET 		0x0B
//#define ID_CALWRITE 	0x0C
#define ID_PARAM 		0x0D
#define ID_POSITION 	0x10
#define ID_ASSERV 		0x11
#define ID_ROBOT 		0x12
#define ID_ACTION 		0x13
#define ID_DETECTION 	0x14
#define ID_IA 			0x15
#define ID_SENSORS 		0x16
#define ID_MOTORS 		0x17
#define ID_EMUL 		0x18
#define ID_OBSTACLE		0x50
#define ID_ACK 			0x90
#define ID_NACK 		0x91

// les réponses de la trame Ack
#define ACK_OK 0
#define ACK_BAD_VERSION 1
#define ACK_IMPOSSIBLE 2
#define ERR_VERSION 0x03 // mauvaise version du robot
#define ERR_SIZE_FRAME 0x04 // mauvaise taille de trame
#define ERR_BAD_ID 0x05 // mauvais identifiant de trame
#define ERR_PARAM_REQ 0x06 // erreur de requete liée aux paramètres
#define ERR_BAD_MOVE 0x07 // mauvais type de déplacement
#define ERR_PARAM_NAME 0x08 // mauvais nom de paramètre

// * Types de déplacement
#define ASS_NUL 0
#define ASS_POLAR 1
#define ASS_POLARREV 2
#define ASS_ROTATION 3
#define ASS_MANUAL 4
#define ASS_PIVOT 6
#define ASS_CIRCLE 7
#define ASS_BLOCK 10

#define HMI_BUTTON_POSITION 0
#define HMI_BUTTON_MOTORS 1
#define HMI_BUTTON_ROBOT 2
#define HMI_BUTTON_ASSERV 3
#define HMI_BUTTON_SENSOR 4
#define HMI_BUTTON_ACTION 5

struct s_Position{
	short posX;
	short posY;
	short posAlpha;
	short spdFor;
	short spdRot;
};
struct s_Asserv{
	char type;
	short tarX;
	short tarY;
	short tarAlpha;
	short spdFor;
	short spdRot;
	char conv;
	char block;
};
struct s_Robot{
	char state;
	char color;
	int count;
	char type;
	char score;
};
struct s_Action{
	char state1;
	char state2;
};
struct s_Motors{
	short spdRight;
	short spdLeft;
	char powerRight;
	char powerLeft;
};

struct s_Sensors{
	int16_t angleRight;
	int16_t angleLeft;
	unsigned char sonFrLeft;
	unsigned char sonFrRight;
	unsigned char sonReLeft;
	unsigned char sonReRight;
};

struct s_Position curPos;
struct s_Asserv curAss;
struct s_Robot curRob;
struct s_Action curAct;
struct s_Motors curMot;
struct s_Sensors curSen;

int encode(char *request);
int decode(char *trame,int t);
void *inputCommand( void*);
void *manageMessages( void* d);
char *TypeNumToChar(int typ);

int sendReceive(char *commande);
char checkSum();
char strWrite[100];
char strRead[50];
int sizeWrite;
int sizeRead;
char logg = 0;
char VersionRobot=17; // 0x01 = 2021 + 0x10=  principal
char calname[20];
char strInput[50];
int inputReady = 1;

pthread_t inp;
pthread_t mess;

char text[50];

char pollTrame[5]={0,0,0,0,0};
void getEntry(GtkEntry *entry, gpointer data);
void getCalValue(GtkEntry *comb, gpointer data);

GtkWidget *PosButton;
GtkWidget *pComboCal;
GtkWidget *pEntryCal;
GtkWidget * pLabelPosPos;
GtkWidget * pLabelPosSpd;
GtkWidget *pDraw;
GtkWidget *pLabelConv;
GtkWidget *pLabelBlock;
GtkWidget *pLabelDetect;
GtkWidget *pLabelAssTar;
GtkWidget *pLabelAssType;
GtkWidget *pLabelMotSpd;
GtkWidget *pLabelMotPow;
GtkWidget *pLabelSenEnc;
GtkWidget *pLabelSenSonar;

GdkRGBA c_red ={1,0,0,0.5};
GdkRGBA c_gray ={0.5,0.5,0.5,0.5};
//,c_blue,c_green;

const char * fiforead = "/tmp/RobBSmap"; // du robot vers l'outil
const char * fifowrite = "/tmp/BSmapRob"; // de l'outil vers le robot
int fd;

char blabla = 1;
