#ifndef PLAY_H
#define PLAY_H

#include "http.h"
#include "song.h"
#include "msgqueue.h"

/* Control commands (state transitions): */
#define MP3_STOP  0x01
#define MP3_PLAY  0x02
#define MP3_PAUSE 0x04
#define MP3_NEXT  0x08
#define MP3_BACK  0x10
/* States: */
#define MP3_STOPPED 0x20    /* initial state */
#define MP3_PLAYING 0x40

#define MP3_EXIT_PAUSE (MP3_STOP | MP3_PLAY | MP3_NEXT | MP3_BACK | MP3_STOPPED | MP3_PLAYING)

/*
 * To show the equaliser on screen
 */
#undef EQ


typedef struct {
  http_t *p;
  song_info_t *song;
} mp3_play_t;

typedef struct playlist_item_tag
{
  http_t *p;
  song_info_t *song;
  struct playlist_item_tag *prev, *next;
} playlist_item_t;


extern int out;
extern msgqueue_t mp3queue;


int mp3_check_state(int state);
void mp3_set_state(int newstate);
void mp3_wait_state(int waitstate);
int mp3_state(void);
int mp3_init(void);

int prev_item(void);
int next_item(void);

int decode_mp3(http_t *p);
int decode_ogg(http_t *p);

song_info_t *get_current_song(void);

#endif

