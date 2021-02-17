#ifndef __BSmap_h__
#define __BSmap_h__
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

#endif
