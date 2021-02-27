#ifndef __BSmap_h__
#define __BSmap_h__


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
#include <wiringPi.h>
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
	uint8_t score;
};
struct s_Action{
	char state1;
	char state2;
};
struct s_Motors{
	short spdRight;
	short spdLeft;
	uint8_t powerRight;
	uint8_t powerLeft;
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
void *manageMessages( void*);

int sendReceive(char *commande);
char checkSum();
char strWrite[100];
char strRead[50];
int sizeWrite;
int sizeRead;
bool log = false;
char VersionRobot=17; // 0x01 = 2021 + 0x10=  principal
char calname[20];
char strInput[50];
bool inputReady = true;

pthread_t inp;
pthread_t mess;

// interface graphique
#define NB_TRAMES 4
#define NB_LABELS 12
#define NB_CAL 32

#define HMI_BUTTON_POSITION 0
#define HMI_BUTTON_ASSERV 1
#define HMI_BUTTON_MOTORS 2
#define HMI_BUTTON_ROBOT 3
#define HMI_BUTTON_ACTION 4

#define HMI_LABEL_POS_POS 0
#define HMI_LABEL_POS_VIT 1
#define HMI_LABEL_ASS_TAR 2
#define HMI_LABEL_ASS_SPD 7
#define HMI_LABEL_MOT_MOT 8


#define HMI_LABEL_SCORE 3
#define HMI_LABEL_STATE 4
#define HMI_LABEL_COLOR 5
#define HMI_LABEL_TIMER 6
//#define HMI_LABEL_TAR_X 7
//#define HMI_LABEL_TAR_Y 8
//#define HMI_LABEL_TAR_ALPHA 9
//#define HMI_LABEL_TAR_TYPE 10

char text[50];

int createHMI(void);
bool pollTrame[NB_TRAMES];
#ifdef GTK_USE
void change(GtkWidget *pb,gpointer data);
const gchar *getTextAssTar();
void getEntry(GtkEntry *entry, gpointer data);
void getCalValue(GtkEntry *comb, gpointer data);
void setCalValue(GtkEntry *entry, gpointer data);
gboolean redraw2(GtkWidget *widget,gpointer userdata);
void on_wondow_main_destroy();

GtkWidget *pWindow;
GtkBuilder *builder;
GtkWidget *window;
GtkWidget *pLabel[20];
GtkWidget *pTable;
GtkWidget *pButton[NB_TRAMES];
GtkWidget *pEntry;
GtkWidget *pEntryCal;
GtkWidget *pComboCal;
GtkWidget *pDraw;
GdkRGBA c_red,c_blue,c_green;
const char *listCal[NB_CAL] = {"COEFF_SPD_FOR","COEFF_SPD_ROT","COEFF_ANG",
	"COEFF_MM","DIST_CONVERGE","ANGLE_CONVERGE","MMSROB_TO_DPSWHL",
	"DPSROB_TO_DPSWHL","KP_FOR","KP_ROT","VERSION","WHEEL_RIGHT",
	"WHEEL_LEFT","SCORE_INIT","ENCODER_LEFT","ENCODER_RIGHT",
	"EXTERNAL_ENCODER","NUMBER_FRONT_SONAR","NUMBER_REAR_SONAR",
	"SONAR_FRONT_LEFT","SONAR_FRONT_RIGHT","SONAR_REAR_LEFT",
	"SONAR_REAR_RIGHT","TIRETTE_HW","TIRETTE_PORT,""TIRETTE_EV3",
	"BAU_HW","BAU_PORT","BAU_EV3","COLOR_HW","COLOR_PORT","COLOR_EV3"};

//cairo_t *cr;
#endif

#endif
