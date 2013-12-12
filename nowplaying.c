#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <linux/unistd.h>

#include "play.h"
#include "msgqueue.h"
#include "song.h"
#include "fonts.h"
#include "msgtypes.h"
#include "video.h"
#include "debug_config.h"

#if DEBUG_NOWPLAYING
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


extern int blinking_icon;


static pthread_mutex_t np_mutex = PTHREAD_MUTEX_INITIALIZER;
static int np_shown = 0;


/*
 *  
 */
int np_check_state(void)
{
    int rc;

    pthread_mutex_lock(&np_mutex);
    rc = np_shown;
    pthread_mutex_unlock(&np_mutex);

    return rc;
}


/*
 *  
 */
static void np_set_state(int state)
{
  pthread_mutex_lock(&np_mutex);
  np_shown = state;
  pthread_mutex_unlock(&np_mutex);
}


/*
 *  nowplaying_draw
 */
void nowplaying_draw(void)
{
    if (np_check_state() == 0) {
        DBG_PRINTF("Skip drawing NOW PLAYING\n");
        return;
    }

    DBG_PRINTF("Now playing Draw\n");

    song_info_t *song = get_current_song();

    if (!song) {
        DBG_PRINTF("Oops no current song\n");
        return;
    }

    video_clear_region(&vid, 0, 0, vid.visx, vid.visy - 95);

    video_no_clipping(&vid);
    video_setpen(&vid, 50, 100);

    if (song->artist) {
        font_write(&font, &vid, song->artist);
    }

    font_write(&font, &vid, "\n");
    video_set_x(&vid, 50);

    if (song->album) {
        font_write(&font, &vid, song->album);
    }

    font_write(&font, &vid, "\n");
    video_set_x(&vid, 50);

    if (song->song) {
        font_write(&font, &vid, song->song);
    }

    song_dump(song);

    blinking_icon = 0;
}


/*
 *
 */
void nowplaying(void)
{
    DBG_PRINTF("Show \"Now playing\"\n");
    np_set_state(1);    /* visible */

    nowplaying_draw();

    for (;;) {
        msg_t *msg = getq(&mainq);

        switch (msg->type) {
        case MSG_REMOTE_STOP:
            putq(&mp3queue, MSG_MP3_STOP, NULL);
            break;

        case MSG_REMOTE_PLAYPAUSE:
            putq(&mp3queue, MSG_MP3_PAUSE, NULL);
            break;

        case MSG_REMOTE_NEXT:
            putq(&mp3queue, MSG_MP3_NEXT, NULL);
            break;

        case MSG_REMOTE_BACK:
            putq(&mp3queue, MSG_MP3_BACK, NULL);
            break;

        default:
            free(msg);
            goto exit;
        }
        free(msg);
    }

exit:

    np_set_state(0); /* gone */

    DBG_PRINTF("Not showing \"Now playing\" anymore\n");

    blinking_icon = 1;
}

