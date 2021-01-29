#ifndef __Remote_h__
#define __Remote_h__

#define STREMOTE_WAITINI 1
#define STREMOTE_WAIT_RECEIVE 2
#define STREMOTE_WAIT_SEND 3

// les identifiants de trame
#define ID_INFO 0x01
#define ID_ORDER 0x02
#define ID_ACK 0x03
#define ID_POSITION 0x10
#define ID_ASSERV 0x11

// les réponses de la trame Ack
#define ACK_OK 0
#define ACK_BAD_VERSION 1
#define ACK_IMPOSSIBLE 2

char VersionRobot=17; // 0x01 = 2021 + 0x10=  principal
#endif
