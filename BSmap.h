#ifndef __Remote_h__
#define __Remote_h__

#define STREMOTE_WAITINI 1
#define STREMOTE_WAIT_RECEIVE 2
#define STREMOTE_WAIT_SEND 3

// les identifiants de trame
#define ID_INFO 0x01
#define ID_ORDER 0x02
#define ID_ACK 0x90
#define ID_POSITION 0x10
#define ID_ASSERV 0x11
#define ID_ROBOT 0x12
#define ID_ACTION 0x13
#define ID_DETECT 0x14
#define ID_IA 0x15
#define ID_SENSORS 0x16
#define ID_MOTORS 0x17
#define ID_EMUL 0x18

// les réponses de la trame Ack
#define ACK_OK 0
#define ACK_BAD_VERSION 1
#define ACK_IMPOSSIBLE 2

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
	char count;
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
char checkSum();
char strWrite[100];
char strRead[50];
int sizeWrite;
int sizeRead;

char VersionRobot=17; // 0x01 = 2021 + 0x10=  principal
#endif
