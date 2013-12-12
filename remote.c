/** Remote.c
 *
 *  Remote driver for Linksys WMA11B
 *
 *  James Pitts.  http://www.turtlehead.co.uk
 *
 *
 *  This is a quick test program to demonstrate remote control
 *  access from the Linksys wma11b.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "remote.h"
#include "msgqueue.h"
#include "msgtypes.h"
#include "play.h"
#include "mixer.h"
#include "nowplaying.h"
#include "debug_config.h"

#if DEBUG_REMOTE
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

extern msgqueue_t mainq;
extern msgqueue_t mixerq;

/* Unit is milliseconds */
#define TIMEOUT		(10*1000)

#define IOCTL_REMOTE_START 0x80045204

#define REMOTE_NONE		0x00000000
#define REMOTE_MENU	 	0x00000002
#define REMOTE_BACK		0x00000004
#define REMOTE_NEXT		0x00000008
#define REMOTE_UP		0x00000010
#define REMOTE_DOWN		0x00000020
#define REMOTE_LEFT		0x00000040
#define REMOTE_RIGHT		0x00000080
#define REMOTE_PLAYPAUSE	0x00000100
#define REMOTE_STOP		0x00000200
#define REMOTE_MUSIC		0x00001000
#define REMOTE_PICTURES		0x00002000
#define REMOTE_SELECT		0x00004000
#define REMOTE_PGUP	 	0x00008000
#define REMOTE_ZOOMIN		0x00010000
#define REMOTE_PGDOWN		0x00040000
#define REMOTE_ZOOMOUT		0x00080000
#define REMOTE_PREVIOUS		0x00100000
#define REMOTE_OPTIONS		0x00200000
#define REMOTE_VOLUMEUP		0x00400000
#define REMOTE_VOLUMEDOWN	0x00800000
#define REMOTE_SETUP		0x01000000

/*  
 *    monitor_remote
 */
#ifdef I386
#include <termios.h>
#include <unistd.h>


/* run as separate thread */
int monitor_remote(void)
{
    struct termios termios_old;
    struct termios termios_new;

    DBG_PRINTF("remote: Keyboard emulation of remote\n");

    /* Set terminal in raw mode: */
    tcgetattr(0, &termios_old);
    termios_new = termios_old;
    cfmakeraw(&termios_new);
    tcsetattr(0, TCSANOW, &termios_new);

    for (;;) {
        int rc;
        char ch;
        struct pollfd p;

        p.fd = 0;
        p.events = POLLIN;
        p.revents = 0;
        rc = poll(&p, 1, TIMEOUT);

        if (rc == 0) {
            /* timed out */
            DBG_PRINTF("remote: timeout\n");
            if ((np_check_state() == 0) && mp3_check_state(MP3_PLAYING)) {
                putq(&mainq, MSG_REMOTE_MUSIC, NULL);
            }
            continue;
        }

        read(0, &ch, sizeof (char));
        switch (ch) {
        case 's':
            putq(&mainq, MSG_REMOTE_DOWN, NULL);
            break;
        case 'w':
            putq(&mainq, MSG_REMOTE_UP, NULL);
            break;
        case 'e':
        case '\r':
            putq(&mainq, MSG_REMOTE_SELECT, NULL);
            break;
        case 'a':
            putq(&mainq, MSG_REMOTE_LEFT, NULL);
            break;
        case 'd':
            putq(&mainq, MSG_REMOTE_RIGHT, NULL);
            break;
        case 'q':
            putq(&mainq, MSG_REMOTE_PREVIOUS, NULL);
            break;
        case 'm':
            putq(&mainq, MSG_REMOTE_MENU, NULL);
            break;
        case 'n':
            putq(&mainq, MSG_REMOTE_NEXT, NULL);
            break;
        case 'b':
        case '\b':
            putq(&mainq, MSG_REMOTE_BACK, NULL);
            break;
        case 'p':
        case ' ':
            putq(&mainq, MSG_REMOTE_PLAYPAUSE, NULL);
            break;
        case 'o':
            putq(&mainq, MSG_REMOTE_STOP, NULL);
            break;
        case 'M':
            putq(&mainq, MSG_REMOTE_MUSIC, NULL);
            break;
        case 'P':
            putq(&mainq, MSG_REMOTE_PICTURES, NULL);
            break;
        case 'r':
            putq(&mainq, MSG_REMOTE_PGUP, NULL);
            break;
        case 'f':
            putq(&mainq, MSG_REMOTE_PGDOWN, NULL);
            break;
        case '=':
            putq(&mixerq, MSG_REMOTE_VOLUMEUP, NULL);
            break;
        case '-':
            putq(&mixerq, MSG_REMOTE_VOLUMEDOWN, NULL);
            break;
        case 'i':
            putq(&mainq, MSG_REMOTE_OPTIONS, NULL);
            break;
        default:
            break;
        }
    }
    /* Reset terminal: */
    tcsetattr(0, TCSANOW, &termios_old);
    exit(1);
}
#else
/* run as separate thread */
int monitor_remote(void)
{
    int fd;

    DBG_PRINTF("remote: starting remote driver\n");

    fd = open("/dev/remote", O_RDONLY);

    if (fd <= 0) {
        DBG_PRINTF("remote: failed to open remote\n");
        return -1;
    }

    DBG_PRINTF("remote: device opened\n");

    fcntl(fd, F_SETFL, O_NONBLOCK);

    DBG_PRINTF("remote: set non-blocking\n");

    if (ioctl(fd, IOCTL_REMOTE_START, NULL) == -1) {
        perror("ioctl\n");
        return 1;
    }

    for (;;) {
        int rc;
        struct pollfd p;

        p.fd = fd;
        p.events = POLLIN;
        p.revents = 0;
        rc = poll(&p, 1, TIMEOUT);

        if (rc == -1) {
            perror("poll\n");

            /* error but continue processing */
            continue;
        }

        if (rc == 0) {
            /* timed out */
            DBG_PRINTF("remote: timeout\n");
            if ((np_check_state() == 0) && mp3_check_state(MP3_PLAYING)) {
                /* JDH - after timeout, move to "now playing" screen */
                putq(&mainq, MSG_REMOTE_MUSIC, NULL);
                continue;
            }
        }

        if ((p.revents & POLLIN) == POLLIN) {
            unsigned int buf;

            if (read(fd, (char *) &buf, sizeof (buf)) == 4) {
                /*DBG_PRINTF("remote: got 0x%X!\n", buf);*/

                switch (buf) {
                case REMOTE_DOWN: 
                    putq(&mainq, MSG_REMOTE_DOWN, NULL);
                    break;
                case REMOTE_UP: 
                    putq(&mainq, MSG_REMOTE_UP, NULL);
                    break;
                case REMOTE_SELECT: 
                    putq(&mainq, MSG_REMOTE_SELECT, NULL);
                    break;
                case REMOTE_LEFT: 
                    putq(&mainq, MSG_REMOTE_LEFT, NULL);
                    break;
                case REMOTE_RIGHT: 
                    putq(&mainq, MSG_REMOTE_RIGHT, NULL);
                    break;
                case REMOTE_PREVIOUS: 
                    putq(&mainq, MSG_REMOTE_PREVIOUS, NULL);
                    break;
                case REMOTE_MENU: 
                    putq(&mainq, MSG_REMOTE_MENU, NULL);
                    break;
                case REMOTE_NEXT: 
                    putq(&mainq, MSG_REMOTE_NEXT, NULL);
                    break;
                case REMOTE_BACK: 
                    putq(&mainq, MSG_REMOTE_BACK, NULL);
                    break;
                case REMOTE_PLAYPAUSE: 
                    putq(&mainq, MSG_REMOTE_PLAYPAUSE, NULL);
                    break;
                case REMOTE_STOP: 
                    putq(&mainq, MSG_REMOTE_STOP, NULL);
                    break;
                case REMOTE_MUSIC: 
                    putq(&mainq, MSG_REMOTE_MUSIC, NULL);
                    break;
                case REMOTE_PICTURES: 
                    putq(&mainq, MSG_REMOTE_PICTURES, NULL);
                    break;
                case REMOTE_PGUP: 
                    putq(&mainq, MSG_REMOTE_PGUP, NULL);
                    break;
                case REMOTE_PGDOWN: 
                    putq(&mainq, MSG_REMOTE_PGDOWN, NULL);
                    break;
                case REMOTE_VOLUMEUP: 
                    putq(&mixerq, MSG_REMOTE_VOLUMEUP, NULL);
                    break;
                case REMOTE_VOLUMEDOWN: 
                    putq(&mixerq, MSG_REMOTE_VOLUMEDOWN, NULL);
                    break;
                case REMOTE_OPTIONS: 
                    putq(&mainq, MSG_REMOTE_OPTIONS, NULL);
                    break;
                }
	            /* add to processing queue */
            }
        }
    }
    close(fd);

    return 0;
}
#endif

