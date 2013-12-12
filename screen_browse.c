/*
 *  screen_browse.c - view artists, albums, genres, playlists
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
#include "playlist.h"
#include "nowplaying.h"
#include "tab.h"
#include "debug_config.h"

#if DEBUG_SCREEN_BROWSE
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


static void walk_playlists(const char *key, void *data, void *arg);
static void walk_artists(const char *key, void *data, void *arg);


#define MAX_TITLESIZE 12

typedef struct _tt {
    tab_t titletab;
    tree_t *(*bfp) (http_t *p, unsigned long sessionid, unsigned long dbid);    /* JDH - broswer function   */
    void (*sfp) (const char *key, void *data, void *arg);                       /* JDH - sorting function   */
    void (*dfp) (void*);                                                        /* JDH - delete function    */
} topics_t;

topics_t topics[4] = {
                        { { 24, 0, 157, 48, "Playlists" },  getPlaylists,   walk_playlists, playlist_delete },
                        { {157, 0, 290, 48, "Artists"   },  browseArtists,  walk_artists,   NULL            },
                        { {290, 0, 423, 48, "Albums"    },  browseAlbums,   walk_artists,   NULL            },    
                        { {423, 0, 556, 48, "Genres"    },  browseGenres,   walk_artists,   NULL            }
                     };


/*
 *  walk_playlists
 */
static void walk_playlists(const char *key, void *data, void *arg)
{
    /* Need to copy because listview will own the data=artist */
    playlist_info_t *playlist;

    DBG_PRINTF("walk_playlists\n");
    playlist = playlist_clone((playlist_info_t *)data);

    listview_add_key((lv_t *) arg, playlist->name, key, playlist);
    /* TODO:: this should be song as the data */
}


/*
 *  walk_artists
 */
static void walk_artists(const char *key, void *data, void *arg)
{
    /* Need to copy because listview will own the data=artist */
    char *artist = strdup(data);

    listview_add_key((lv_t *) arg, artist, key, artist);
    /* TODO:: this should be song as the data */
}


/*
 *  browse_screen - browse database by various attributes
 */
void browse_screen(http_t *p, int dbid, long sessionid)
{
    lv_t al;
    tree_t *artists;
    song_filter_t song_filter;
    unsigned int ti;    /* JDH - topic index */
    //tree_t *playlists;

    DBG_PRINTF("browse_screen\n");
    clear_tab_region();
    //draw_header(topics[ti].title);  /* JDH - indicate the topic we're viewing */
    for (ti = 1; ti < 4; ti++) {
        draw_inactive_tab(topics[ti].titletab);
    }
    ti = 0;
    draw_active_tab(topics[ti].titletab);
//    artists = getArtists(p, sessionid, dbid);
    artists = topics[ti].bfp(p, sessionid, dbid);
    //artists = browseGenres(p, sessionid, dbid);
    //playlists = getPlaylists(p, sessionid, dbid);
    DBG_PRINTF("listview_init\n");
    listview_init(&al, 25, 50, vid.visx - 50, vid.visy - 97 - 50); 
    DBG_PRINTF("tree_walk\n");
    tree_walk(artists, topics[ti].sfp, &al);
    //tree_walk(playlists, walk_playlists, &al);
    DBG_PRINTF("tree_delete\n");
    tree_delete(artists, topics[ti].dfp);
    //tree_delete(playlists, playlist_delete);
    //listview_dump(&al);
    listview_draw(&al);

    for (;;) {
        DBG_PRINTF("get new msg\n");
        msg_t *msg = getq(&mainq);
        int page = 10;

        switch (msg->type) {
        case MSG_REMOTE_PREVIOUS:
            DBG_PRINTF("go back\n");
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

        case MSG_REMOTE_SELECT:
            {
                memset(&song_filter, 0, sizeof(song_filter_t));
	            //char *artist = listview_select(&al);
	            //playlist_info_t *playlist = listview_select(&al);

                switch (ti) {
                case 0:
                {
                    playlist_info_t *playlist = listview_select(&al);
    	            DBG_PRINTF("artist selected %s\n", playlist->name);
                    song_filter.playlist_id = playlist->id;
                    break;
                }
                case 1:
                    song_filter.artist = listview_select(&al);
    	            DBG_PRINTF("artist selected %s\n", song_filter.artist);
                    break;
                case 2:
                    song_filter.album = listview_select(&al);
    	            DBG_PRINTF("album selected %s\n", song_filter.album);
                    break;
                case 3:
                    song_filter.genre = listview_select(&al);
    	            DBG_PRINTF("genre selected %s\n", song_filter.genre);
                    break;
                default:
                    DBG_PRINTF("browse_screen: something went really wrong.\n");
                }

                clear_tab_region();
	            draw_header("Wait a moment...");
	            //song_screen(p, dbid, sessionid, 0, artist, NULL);   /* JDH - 0 will need to become a plid if playlist selected */
	            song_screen(p, dbid, sessionid, song_filter);
	            //song_screen(p, dbid, sessionid, playlist->id, NULL, NULL, NULL);   /* JDH - 0 will need to become a plid if playlist selected */

	            DBG_PRINTF("back from song list\n");
	            /* Keep old list of artists "al"! */
	            /*listview_find_closest ( &al, artist );*/
	            //draw_header("Artists");
                clear_tab_region();
                draw_active_tab(topics[ti].titletab);
	            //draw_header(topics[ti].title);
	            listview_draw(&al);
	            break;
            }

        case MSG_REMOTE_LEFT:
            draw_inactive_tab(topics[ti].titletab);
            ti = (ti - 1) & 0x03;
            listview_free(&al, NULL);
            //draw_header(topics[ti].title);  /* JDH - indicate the topic we're viewing */
            draw_active_tab(topics[ti].titletab);
            artists = topics[ti].bfp(p, sessionid, dbid);
            DBG_PRINTF("listview_init\n");
            listview_init(&al, 25, 50, vid.visx - 50, vid.visy - 97 - 50); 
            DBG_PRINTF("tree_walk\n");
            tree_walk(artists, topics[ti].sfp, &al);
            DBG_PRINTF("tree_delete\n");
            tree_delete(artists, topics[ti].dfp);
            listview_draw(&al);
            break;

        case MSG_REMOTE_RIGHT:
            draw_inactive_tab(topics[ti].titletab);
            ti = (ti + 1) & 0x03;
            listview_free(&al, NULL);
            //draw_header(topics[ti].title);  /* JDH - indicate the topic we're viewing */
            draw_active_tab(topics[ti].titletab);
            artists = topics[ti].bfp(p, sessionid, dbid);
            DBG_PRINTF("listview_init\n");
            listview_init(&al, 25, 50, vid.visx - 50, vid.visy - 97 - 50); 
            DBG_PRINTF("tree_walk\n");
            tree_walk(artists, topics[ti].sfp, &al);
            DBG_PRINTF("tree_delete\n");
            tree_delete(artists, topics[ti].dfp);
            listview_draw(&al);
            break;
#if 0
            {
	            char *artist = listview_select(&al);

	            draw_header("Wait a moment...");
	            album_screen(p, dbid, sessionid, artist);

	            /* Keep old list of artists "al"! */
	            /*listview_find_closest ( &al, artist );*/
	            draw_header("Artists");
	            listview_draw(&al);
	            break;
            }
#endif

        case MSG_REMOTE_MUSIC:
            {
	            nowplaying();
	            clear_tab_region();
	            draw_header("Artists");
	            listview_draw(&al);
	            break;
            }
        }
        DBG_PRINTF("free msg\n");
        free(msg);
    }

exit:
    DBG_PRINTF("free artist list\n");

    listview_free(&al, NULL);

    DBG_PRINTF("quit artist screen\n");

    return;
}

