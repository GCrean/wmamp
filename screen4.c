/**  Screen 4
 *
 *   Song selection
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "video.h"
#include "http.h"
#include "daap.h"
#include "msgqueue.h"
#include "msgtypes.h"
#include "commands.h"
#include "listview.h"
#include "play.h"
#include "song.h"
#include "dialog.h"
#include "nowplaying.h"
#include "screen.h"
#include "debug_config.h"

#if DEBUG_SCREEN4
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


/*
 *  walk_songs
 */
static void walk_songs(const char *key, void *data, void *arg)
{
    lv_t *lv = (lv_t*) arg;
    song_info_t *songinfo = song_clone((song_info_t *) data);
    char sortkey[128];

    sprintf(sortkey, "%d %s", songinfo->track, songinfo->song);
    /*sprintf (sortkey, "%d", songinfo->track); */
    if (listview_add_key(lv, songinfo->song, sortkey, songinfo) == -1) {
        DBG_PRINTF("screen4: dup list item\n");
    }

    /*listview_dump (lv);*/
}


static mp3_play_t *
mp3_item(http_t *p, song_info_t *songinfo)
{
    mp3_play_t *mp3_play;

    mp3_play = malloc(sizeof(mp3_play_t));
    mp3_play->p = p;
    mp3_play->song = song_clone(songinfo);

    return mp3_play;
}


/*
 *  song_screen
 */
void song_screen(http_t *p,
                    int dbid,
                    long sessionid,
                    song_filter_t sfilter)
{
    lv_t sl;
    tree_t *songs;

    songs = getSongsByArtistAlbumPlaylist(p, sessionid, dbid, sfilter);
  
    listview_init(&sl, 25, 50, vid.visx - 50, vid.visy - 97 - 50); 
    tree_walk(songs, walk_songs, &sl);
    tree_delete(songs, song_delete);
    draw_header("Songs");
    listview_draw(&sl);

    for (;;) {
        msg_t *msg = getq(&mainq);
        int page = 10;

        switch (msg->type) {
        case MSG_REMOTE_PREVIOUS:
        case MSG_REMOTE_LEFT:
            free(msg);
            goto exit;

        case MSG_REMOTE_UP:
            listview_up(&sl);
            break;

        case MSG_REMOTE_DOWN:
            listview_down(&sl);
            break;

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

        case MSG_REMOTE_PGUP:
            page = -10;
            /* FALLTHROUGH */
        case MSG_REMOTE_PGDOWN:
            {
                int sdist, ddist;

                sl.selected = listview_move_n(sl.selected, page, &sdist);
                sl.draw     = listview_move_n(sl.draw, sdist, &ddist);
                if (sdist != ddist) {
                    sl.distance += sdist - ddist;
                }
                listview_draw(&sl);
                break;
            }

        case MSG_REMOTE_RIGHT:  /* right adds to play list.. */
            {
                mp3_play_t *mp3_play;
                song_info_t *songinfo = listview_select(&sl);

                DBG_PRINTF("request: add to playlist %d\n", songinfo->id);
                mp3_play = mp3_item(p, songinfo);
                putq(&mp3queue, MSG_MP3_ADD, mp3_play);

                putq(&mp3queue, MSG_MP3_PLAY, p);
                break;
            }

        case MSG_REMOTE_SELECT:
            {
                mp3_play_t *mp3_play;
                lv_node_t *next;
                song_info_t *songinfo;

                putq(&mp3queue, MSG_MP3_STOP, NULL);
                putq(&mp3queue, MSG_MP3_CLEAR, NULL);

                songinfo = listview_select(&sl);
                DBG_PRINTF("request: add to playlist %d\n", songinfo->id);
                mp3_play = mp3_item(p, songinfo);
                putq(&mp3queue, MSG_MP3_ADD, mp3_play);

                for (next = listview_next(&sl); next; next = listview_next_i(next)) {
                    songinfo = next->data;
                    DBG_PRINTF("request: add to playlist %d\n", songinfo->id);
                    mp3_play = mp3_item(p, songinfo);
                    putq(&mp3queue, MSG_MP3_ADD, mp3_play);
                }

                putq(&mp3queue, MSG_MP3_PLAY, p);
                break;
            }

        case MSG_REMOTE_MUSIC:
            {
                nowplaying();
                draw_header("Songs");
                listview_draw(&sl);
                break;
            }

        case MSG_MP3_ERROR:
            {
                dialog_box("Connection and login\n"
                           "to server failed.\n"
                           "Is server still running\n"
                           "or does it already have\n"
                           "too many connections\n"
                           "active?",
                           DIALOG_OK);
                break;
            }
        }
        free(msg);
    }

exit:  

    listview_free(&sl, song_delete);
}

