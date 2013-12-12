/**  Screen 3
 *
 *   Album selection
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "screen.h"
#include "video.h"
#include "http.h"
#include "daap.h"
#include "msgqueue.h"
#include "msgtypes.h"
#include "commands.h"
#include "listview.h"
#include "song.h"
#include "nowplaying.h"
#include "debug_config.h"

#if DEBUG_SCREEN3
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
 *  walk_albums
 */
static void walk_albums(const char *key, void *data, void *arg)
{
    char *album = strdup(data);

    listview_add_key((lv_t *) arg, album, key, album);
    /* TODO: album should be *song */
}


/*
 *  album_screen
 */
void album_screen(http_t *p, int dbid, long sessionid, const char *artist)
{
    lv_t al;
    tree_t *albums;

    DBG_PRINTF("Artist %s\n", artist);

    albums = getAlbumsByArtists(p, sessionid, dbid, artist);
    listview_init(&al, 25, 50, vid.visx - 50, vid.visy - 97 - 50); 
    tree_walk(albums, walk_albums, &al);
    tree_delete(albums, NULL);
    draw_header("Albums");
    listview_draw(&al);

    for (;;) {
        msg_t *msg = getq(&mainq);
        int page = 10;

        switch (msg->type) {
        case MSG_REMOTE_PREVIOUS:
        case MSG_REMOTE_LEFT:
            free(msg);
            goto exit;

        case MSG_REMOTE_UP:
            listview_up(&al);
            break;

        case MSG_REMOTE_DOWN:
            listview_down(&al);
            break;

        case MSG_REMOTE_PGUP:
            page = -10;
            /* FALLTHROUGH */
        case MSG_REMOTE_PGDOWN:
            {
	            int sdist, ddist;

        	    al.selected = listview_move_n(al.selected, page, &sdist);
	            al.draw = listview_move_n(al.draw, sdist, &ddist);
	            if (sdist != ddist) {
	                al.distance += sdist - ddist;
                }
	            listview_draw(&al);
	            break;
            }

        case MSG_REMOTE_RIGHT:
        case MSG_REMOTE_SELECT:
            {
                song_filter_t song_filter;
	            char *album = listview_select(&al);

	            //DBG_PRINTF("album selected %s\n", album);
                memset(&song_filter, 0, sizeof(song_filter_t));
                song_filter.artist = artist;
                song_filter.album = listview_select(&al);
	            DBG_PRINTF("album selected %s\n", song_filter.album);

	            draw_header("Wait a moment...");
	            song_screen(p, dbid, sessionid, song_filter);

	            /* Keep old list of albums "al"! */
	            /*listview_find_closest ( &al, album );*/
	            draw_header("Albums");
	            listview_draw(&al);
	            break;
            }

        case MSG_REMOTE_MUSIC:
            {
	            nowplaying();
	            draw_header("Albums");
	            listview_draw(&al);
	            break;
            }
        }
        free (msg);
    }

exit:

    listview_free(&al, NULL);
}

