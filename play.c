/* man didn't we just hack this to shreds? */
/* stolen and hacked within an inch of its life from minimad, */
/* the example libmad player. */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <errno.h>

#include "mad.h"
#include "screen.h"
#include "hexdump.h"
#include "http.h"
#include "play.h"
#include "commands.h"
#include "msgtypes.h"
#include "msgqueue.h"
#include "dialog.h"
#include "nowplaying.h"
#include "debug_config.h"

#if DEBUG_PLAY_CONTROL
/*
 * This macro definition is a GNU extension to the C Standard. It lets us pass
 * a variable number of arguments and does some special magic for those cases
 * when only a string is passed. For a more detailed explanation, see the gcc
 * manual here: http://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Variadic-Macros.html#Variadic-Macros
 */
#define DBG_PRINTF(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define DBG_PRINTF(format, ...)
#endif


/* Maximum number of songs on the playlist. */
/* JDH - limits suck. need to find a way to make this irrelevant */
#define MAX_SONGS	100

#define AUDIO_DEV	"/dev/dsp"


static void *play(void *arg);

msgqueue_t mp3queue;

#if 0
static const char *state_str[] = {
  "<UNDEFINED>",
  "MP3_STOP",
  "MP3_PLAY",
  "MP3_PAUSE",
  "MP3_NEXT",
  "MP3_BACK",
  "MP3_STOPPED",
  "MP3_PLAYING"
};
#endif

/* AUDIO_DEV file descriptor: */
int out = -1;

static int play_state = MP3_STOPPED;
static pthread_cond_t state_cond; /* condition variable */
static pthread_mutex_t mp3_mutex;

static playlist_item_t *head = NULL; /* start of playlist */
static playlist_item_t *tail = NULL; /* end of playlist */
static playlist_item_t *curr = NULL; /* current song */
static int nr_songs = 0;		/* numbers of songs on playlist */


/* mp3_change_state(MP3_STOP) */
static void mp3_stop(void)
{
    pthread_mutex_lock(&mp3_mutex);
    if (play_state != MP3_STOPPED) {
        play_state = MP3_STOP;
    }
    DBG_PRINTF("play: (stop) %04X\n", play_state);
    pthread_cond_signal(&state_cond);
    pthread_mutex_unlock(&mp3_mutex);
}


/* mp3_change_state(MP3_PAUSE) */
static void mp3_pause(void)
{
    pthread_mutex_lock(&mp3_mutex);
    /* JDH states are bits now */
    if (play_state & (MP3_PLAYING | MP3_PAUSE)) {   /* JDH - don't do pause/play if we aren't paused or playing */
        play_state = (play_state & MP3_PAUSE) ? MP3_PLAY : MP3_PAUSE;
    }
    DBG_PRINTF("play: (pause) %04X\n", play_state);
    pthread_cond_signal(&state_cond);
    pthread_mutex_unlock(&mp3_mutex);
}


void mp3_set_state(int newstate)
{
    pthread_mutex_lock(&mp3_mutex);
    play_state = newstate;
    DBG_PRINTF("play: (set) %04X\n", play_state);
    pthread_cond_signal(&state_cond);
    pthread_mutex_unlock(&mp3_mutex);
}


/*
 *  mp3_wait_state
 */
void mp3_wait_state(int waitstates)
{
    pthread_mutex_lock(&mp3_mutex);
    DBG_PRINTF("play: (waiting for %04X) %04X\n", waitstates, play_state);
    /* JDH states are bits now, so waitstates is a mask) */
    while (!(play_state & waitstates)) {
        pthread_cond_wait(&state_cond, &mp3_mutex);
    }
    DBG_PRINTF("play: (waited) %04X\n", play_state);
    pthread_mutex_unlock(&mp3_mutex);
}


int mp3_state(void)
{
    int rc;

    pthread_mutex_lock (&mp3_mutex);
    rc = play_state;
    pthread_mutex_unlock (&mp3_mutex);

    return rc;
}


/*
 *  mp3_check_state
 */
int mp3_check_state(int state)
{
    int rc;

    pthread_mutex_lock (&mp3_mutex);
    /* JDH states are bits now */
    rc = (play_state & state);
    pthread_mutex_unlock (&mp3_mutex);

    return rc;
}


/* Assumes head != NULL. */
static void pop_song(void)
{
    playlist_item_t *next = head->next;

    song_delete(head->song);
    free(head);
    head = next;
    nr_songs--;
}


/*
 *  add
 */
static void add(song_info_t *song)
{
    playlist_item_t *item;

    DBG_PRINTF("play: adding song %d to playlist\n", song->id);

    pthread_mutex_lock(&mp3_mutex);

    item = malloc(sizeof (*item));
    item->song = song;
    item->prev = tail;		/* possibly NULL */
    item->next = NULL;

    if (head) {			/* implies tail != NULL */
        if (nr_songs > MAX_SONGS) {
            /* Avoid curr getting off the playlist: */
            if (curr == head) {
    	        curr = head->next;
	        }
            /* Dequeue `oldest' song on playlist: */
            pop_song();
        }
        tail->next = item;
    }
    else {			/* first item */
        head = item;
    }
    tail = item;
    /* Make sure curr is on the playlist: */
    if (!curr) {
        curr = head;
    }
    nr_songs++;

    pthread_mutex_unlock(&mp3_mutex);
}


/*
 *  mp3_clear
 */
static void mp3_clear(void)
{
    pthread_mutex_lock(&mp3_mutex);

    while (head) {
        pop_song();
    }

    /* head == NULL */
    curr = tail = NULL;
    nr_songs = 0;

    pthread_mutex_unlock(&mp3_mutex);
}


/*
 *  get_current_song
 */
song_info_t *get_current_song(void)
{
    song_info_t *song;

    pthread_mutex_lock(&mp3_mutex);
    song = (curr) ? curr->song : NULL;
    pthread_mutex_unlock(&mp3_mutex);

    return song;
}


/*
   1 get next shuffle song id
   songid = shuffle_next();

   song_info_t *s;
   s = calloc (1, sizeof (song_info_t));
   s->song = strdup("Shuffle Play");
   s->id = songid;

   2 add as dummy song to playlist
   add(s);

   need http_t *p. (screen2,3,4)

   3 prepare MSG_MP3_PLAY message with empty song data
   putq (&mp3queue, MSG_MP3_PLAY, p);
*/

static unsigned int shuffle_next(void)
{
    unsigned int n = song_db.nr_songs;
    unsigned int i = song_db.curr;

    if (i == n) {
        /* Restart from scratch (modulo): */
        i = song_db.curr = 0;
    }

    if (i < n) {
        /* Select element to swap from array[i..n-1] */
        unsigned int j = i + rand() / (RAND_MAX / (n - i));
        unsigned int t = song_db.songids[j];

        song_db.songids[j] = song_db.songids[i];
        song_db.songids[i] = t;
    }

    return song_db.songids[song_db.curr++];
}


int prev_item(void)
{
    int rc;

    pthread_mutex_lock(&mp3_mutex);

    if (curr) {
        if (curr->prev) {
            curr = curr->prev;
        }
        else {
            curr = tail;    /* JDH - move to the bottom of the list, presumably tail is non-NULL */
        }
        rc = 1;
    }
    else {
        rc = 0;
    }

    pthread_mutex_unlock(&mp3_mutex);

    return rc;
}


int next_item(void)
{
    int rc;

    pthread_mutex_lock(&mp3_mutex);

    if (curr) {
        if (curr->next) {
            curr = curr->next;
        }
        else {
            curr = head;    /* JDH - move to the top of the list, presumably head is non-NULL */
        }
        rc = 1;
    }
    else {
        rc = 0;
    }

    pthread_mutex_unlock(&mp3_mutex);

    return rc;
}


/*
 *  mp3_msg_loop
 */
static void mp3_msg_loop(void)
{
    DBG_PRINTF("play: msg processing loop started\n");

    for (;;) {
        msg_t *msg = getq(&mp3queue);

        DBG_PRINTF("play: request %d\n", msg->type);

        switch (msg->type) {
        case MSG_MP3_ADD:
            {
                mp3_play_t *p = (mp3_play_t *)msg->data;

                add(p->song);	/* inherits song data (no copy) */
                free(p);
                break;
            }

        case MSG_MP3_PLAY:
            {
	            http_t *p = (http_t *) msg->data;

	            if (!mp3_check_state(MP3_PLAYING)) {
	                pthread_t playthread;

	                pthread_create(&playthread, NULL, play, p);

	                int policy = SCHED_OTHER;
	                struct sched_param param;

	                pthread_getschedparam(playthread, &policy, &param);

#if DEBUG_PLAY_CONTROL
	                int min = sched_get_priority_min(SCHED_FIFO);
#endif
	                int max = sched_get_priority_max(SCHED_FIFO);

	                DBG_PRINTF("play: sched priority %d %d\n", min, max);
	                param.sched_priority = max;

	                int rc = pthread_setschedparam(playthread, SCHED_FIFO, &param);

	                if (rc != 0) {	/* not an issue if this fails... */
	                    errno = rc;
	                    perror("play: pthread_setschedparam");
	                }

	                /* Turn it into a free running thread: */
	                pthread_detach(playthread);
	            }
	            else {
	                DBG_PRINTF("play: already playing\n");
                }
	            break;
            }

        case MSG_MP3_PAUSE:		/* also when pressing stop button */
            mp3_pause();
            break;

        case MSG_MP3_NEXT:
            /* FALLTHROUGH */
        case MSG_MP3_BACK:
#if 0
            /*
             * JDH - this causes a fragment of the paused song to play before changing songs
             * which is very annoying. Also, when paused we no longer have to transition to
             * play before changing to a new state.
             */
            if (mp3_check_state(MP3_PAUSE)) {
                mp3_set_state(MP3_PLAY);
                mp3_wait_state(MP3_PLAYING);
            }
#endif
            mp3_set_state((msg->type == MSG_MP3_NEXT) ? MP3_NEXT : MP3_BACK);
            break;

        case MSG_MP3_STOP:		/* when selecting a new song */
            mp3_stop();
            mp3_wait_state(MP3_STOPPED);
            break;

        case MSG_MP3_CLEAR:
            mp3_stop();   /* JDH - shouldn't we wait for MP3_STOPPED here too? */
            mp3_clear();
            DBG_PRINTF("play: CLEARED\n");
            break;
        }
        free(msg);
    }
    DBG_PRINTF("play: msg processing loop stopped\n");
}


/*
 *  audio_open
 */
static int audio_open(void)
{
    int r;

    DBG_PRINTF("play: Opening Device\n");

    out = open(AUDIO_DEV, O_WRONLY);

    if (out == -1) {
        perror("play: Open dsp");
        return -1;
    }

    /*ioctl (out, SNDCTL_DSP_RESET, 0);*/

    /* Signed 16 bit little endian. */
    r = AFMT_S16_LE;
    ioctl(out, SNDCTL_DSP_SETFMT, &r);
    r = 1;
    ioctl(out, SNDCTL_DSP_STEREO, &r);
    r = 44100;
    ioctl(out, SNDCTL_DSP_SPEED, &r);

    return 0;
}


/*
 *  audio_close
 */
static int audio_close(void)
{
    DBG_PRINTF("play: Closing Device\n");
    fdatasync(out);
    ioctl(out, SOUND_PCM_SYNC, 0); 
    close(out);
    out = -1;
    DBG_PRINTF("play: Closing Device done\n");

    return 0;
}


/* 
 *  mp3_init
 */
int mp3_init(void)
{
    msgqueue_init(&mp3queue);
    pthread_t mp3thread;

    DBG_PRINTF("play: (init) %04X\n", play_state);

    /* Create mutex and condition variable: */
    pthread_mutex_init(&mp3_mutex, NULL);
    pthread_cond_init(&state_cond, NULL);

    /* Create thread: */
    pthread_create(&mp3thread, NULL, (void *) mp3_msg_loop, NULL);

    return 0;
}


static int
play_aux(http_t *h, unsigned int songid)
{
    char mp3_name[128];
    long sessionid = login(h);

    /* Login to http server and get sessionid: */
    if (sessionid == -1) {
        putq(&mainq, MSG_MP3_ERROR, NULL);
        DBG_PRINTF("play: Login failed\n");
        http_close(h);
        /*mp3_stop();*/
        return 0;
    }

    /* Get the database id: */
    /* unsigned long dbid = database (h, sessionid); */
    /* Let's assume it's globally available. See screen2.c */

    /* Get the song "<songid>.mp3": */
    sprintf(mp3_name, "%u.mp3", songid);
    getSong(h, sessionid, dbid, mp3_name);

    switch (h->type) { 
    case CT_OGG:
        decode_ogg(h);
        break;
    case CT_MP3:
    default:
        decode_mp3(h);
        break;
    }

    http_close(h);
  
    /* just reconnect to log back out.... ;-/ */
    http_reconnect(h);

    sendLogout(h, sessionid);

    http_close(h);
    return 1;
}


/*
 *  play, runs in separate thread.
 */
static void *play(void *arg)
{
    http_t *p = (http_t *) arg;
    http_t hrec;
    http_t *h = &hrec;

    DBG_PRINTF("play: Starting to play songs on playlist\n");

    if (out != -1) {
        DBG_PRINTF("play: Audio accidentally left open; closing it\n");
        audio_close();
    }

    audio_open();

    do {
        song_info_t *song = get_current_song();

        if (!song) {
            DBG_PRINTF("play: Oops empty playlist \n");
            break;
        }

        /* Must insist on this; some thread might be waiting for it! */
        mp3_set_state(MP3_PLAYING);

        nowplaying_draw();

        http_clone(h, p);

        if (!play_aux(h, song->id)) {
            break;
        }

    } while (!mp3_check_state(MP3_STOP));

    audio_close();

    mp3_set_state(MP3_STOPPED);

    /* Keep whatever is on the playlist. */
    /*mp3_clear ();*/

    DBG_PRINTF("play: Play thread exited\n");
  
    /* This will terminate the thread. */
    return 0;
}

