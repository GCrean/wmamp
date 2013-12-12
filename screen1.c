/**  Screen 1
 *
 *   Enables selection of music server 
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "timer.h"
#include "discover.h"
#include "screen.h"
#include "http.h"
#include "msgqueue.h"
#include "msgtypes.h"
#include "commands.h"
#include "listview.h"
#include "dialog.h"
#include "debug_config.h"

#if DEBUG_SCREEN1
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


static
void shuffle_timer_cb(int arg)
{
    DBG_PRINTF("Shuffle timer expired\n");
    putq(&mainq, MSG_REMOTE_RIGHT, NULL);
}


static
int server_connect(struct in_addr addr,
                   unsigned short int port,
                   http_t *p,
                   unsigned long *dbid,
                   long *sessionid)
{
    int rc;

    DBG_PRINTF("connecting to %s:%d\n", inet_ntoa(addr), ntohs(port));

    memset(p, 0, sizeof(http_t));
    rc = http_open(p, addr, port);

    if (rc == -1) {
        char error[256];
        sprintf(error, "Failed to connect to\n server (%s)\n", inet_ntoa(addr));
        dialog_box(error, DIALOG_OK);
        return 1;
    }

#if 0
    /* Not using any of this just now. Might as well skip it. */
    getContentCodes(p);
    getServerInfo(p);
#endif

    *sessionid = login(p);

    if (*sessionid == -1) {
        char error[256];
        sprintf(error, "Login to server\n(%s) failed\n", inet_ntoa(addr));
        dialog_box(error, DIALOG_OK);
        http_close(p);
        return 1;
    }

    *dbid = database(p, *sessionid);

    return 0;
}

/*  
 *    screen1
 */
void screen1(void)
{
    lv_t lv;
    pthread_t discover_thread;
    discover_t dis;
    http_t p;
    long sessionid;

restart:
    draw_header("DAAP Servers");
    listview_init(&lv, 25, 50, vid.visx - 50, vid.visy - 97 - 50);
    if (discover_init(&dis) != 0) {
        DBG_PRINTF("error creating discovery\n");
    }
    /* Start discover thread; message replies: FOUND_SERVER, REMOVE_SERVER */
    pthread_create(&discover_thread, NULL, (void*) discover_run, &dis);
    listview_draw(&lv);

    for (;;) {

        /* Start shuffle timer: */
        timer_start(TIMER_SHUFFLE, 100, 1, shuffle_timer_cb);

        msg_t *msg = getq(&mainq);

        /* Stop shuffle timer if still ticking. */
        timer_stop(TIMER_SHUFFLE);

        switch (msg->type) { 
        case FOUND_SERVER:
            {
                foundserver_t *s = (foundserver_t *) msg->data;

                DBG_PRINTF("Found server \"%s\" at %s:%d\n", s->name, inet_ntoa(s->addr), ntohs(s->port));

                listview_add(&lv, s->name, s);
                listview_draw(&lv);
                /* Don't free (msg->data)! s=msg->data is owned by listview. */
        break;
            }

        case REMOVE_SERVER:
            {
                removeserver_t *s = (removeserver_t*) msg->data;

                listview_remove(&lv, s->name);
                listview_draw(&lv);
                free (msg->data);
                break;
            }

        case MSG_REMOTE_DOWN:
            listview_down(&lv);
            break;

        case MSG_REMOTE_UP:
            listview_up(&lv);
            break;

        case MSG_REMOTE_SELECT:
#if 0
            {
                foundserver_t *s = listview_select(&lv);
                struct in_addr addr;
                unsigned short int port;

                free(msg);
                if (!s) {
                    /* Mmm, is this really what we want? */
                    goto exit;
                }

                discover_stop(&dis);
                pthread_join(discover_thread, NULL);

                /* Need copy because of subsequent listview_free. */
                addr.s_addr = s->addr.s_addr;
                port = s->port;

                listview_free(&lv, NULL);

                draw_header("Wait a moment...");

                /* apparently there is some global dbid we're stuffing here */
                if (server_connect(addr, port, &p, &dbid, &sessionid)) {
                    /* JDH - failed to connect */
                    goto restart;
                }

                DBG_PRINTF("server selected %s\n", s->name);

                song_screen(&p, dbid, sessionid, 0, NULL, NULL, NULL);

                sendLogout(&p, sessionid);
                http_close(&p);

                goto restart;
            }
#endif
        case MSG_REMOTE_RIGHT:
            {
                foundserver_t *s = listview_select(&lv);
                struct in_addr addr;
                unsigned short int port;

                free(msg);
                if (!s) {
                    /* Mmm, is this really what we want? */
                    goto exit;
                }

                discover_stop(&dis);
                pthread_join(discover_thread, NULL);

                /* Need copy because of subsequent listview_free. */
                addr.s_addr = s->addr.s_addr;
                port = s->port;

                listview_free(&lv, NULL);

                draw_header("Wait a moment...");

                /* apparently there is some global dbid we're stuffing here */
                if (server_connect(addr, port, &p, &dbid, &sessionid)) {
                    /* JDH - failed to connect */
                    goto restart;
                }

                DBG_PRINTF("go to artist screen\n");

                browse_screen(&p, dbid, sessionid);

                DBG_PRINTF("back from artist screen\n");
                sendLogout(&p, sessionid);
                DBG_PRINTF("logged out\n");
                http_close(&p);

                goto restart;
            }

        case MSG_REMOTE_OPTIONS:
            DBG_PRINTF("Exiting server selection screen\n");
            free(msg);
            goto exit;

        case MSG_REMOTE_LEFT:
        case MSG_REMOTE_PREVIOUS:
        case MSG_REMOTE_MENU:
        case MSG_REMOTE_NEXT:
        case MSG_REMOTE_BACK:
        case MSG_REMOTE_PLAYPAUSE:
        case MSG_REMOTE_STOP:
        case MSG_REMOTE_MUSIC:
        case MSG_REMOTE_PICTURES:
        case MSG_REMOTE_PGUP:
        case MSG_REMOTE_PGDOWN:
        case MSG_REMOTE_VOLUMEUP:
        case MSG_REMOTE_VOLUMEDOWN:
            break;

        default:
            DBG_PRINTF("[screen1]: Unexpected message\n");
            break;
        }
        free (msg);
    }

exit:

    DBG_PRINTF("[screen1]: Stopping\n");

    discover_stop(&dis);
    pthread_join(discover_thread, NULL);
    listview_free(&lv, NULL);

    cleanmsgqueue(&mainq);

    DBG_PRINTF("[screen1]: Done.\n");
}

