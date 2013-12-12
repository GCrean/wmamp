#ifndef MSGIDS_H
#define MSGIDS_H

#include "msgqueue.h"
#include "http.h"

#define FOUND_SERVER 1
typedef struct {
    char name[64];
    struct in_addr addr;
    unsigned short int port;
} foundserver_t;

#define REMOVE_SERVER 2
typedef struct {
    char name[64];
} removeserver_t;

#define MSG_REMOTE_DOWN        3
#define MSG_REMOTE_UP          4
#define MSG_REMOTE_SELECT      5
#define MSG_REMOTE_LEFT        6
#define MSG_REMOTE_RIGHT       7
#define MSG_REMOTE_PREVIOUS    8
#define MSG_REMOTE_MENU	       9
#define MSG_REMOTE_NEXT       10
#define MSG_REMOTE_BACK       11
#define MSG_REMOTE_PLAYPAUSE  12
#define MSG_REMOTE_STOP       13
#define MSG_REMOTE_MUSIC      14
#define MSG_REMOTE_PICTURES   15
#define MSG_REMOTE_PGUP       16
#define MSG_REMOTE_PGDOWN     17
#define MSG_REMOTE_VOLUMEUP   18
#define MSG_REMOTE_VOLUMEDOWN 19
#define MSG_REMOTE_OPTIONS    20

/* mp3 message queue commands */
#define MSG_MP3_PLAY          100
#define MSG_MP3_STOP          101
#define MSG_MP3_PAUSE         102
#define MSG_MP3_ADD           103
#define MSG_MP3_CLEAR         104
#define MSG_MP3_NEXT          105
#define MSG_MP3_BACK          106
#define MSG_MP3_ERROR         107

#endif
