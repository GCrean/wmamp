/** Remote.c
 *
 *  Mixer driver for Linksys WMA11B
 *
 *  Andrew Webster  
 *
 *  First try, so please be forgive the mess...
 *  Controlling the mixer on the wma11b
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "mixer.h"
#include "msgtypes.h"
#include "msgqueue.h"
#include "bitmap.h"
#include "timer.h"
#include "debug_config.h"

#if DEBUG_MIXER
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

#define MIXER_DEV "/dev/mixer"

#define VOLUME_MIN	0
#define VOLUME_MAX	63
#define VOLUME_MUTE	0
#define VOLUME_STEP	4

/* OFfset from bottom right */
#define VOLUME_Y_OFFSET	-10
#define VOLUME_X_OFFSET 64


static pthread_cond_t state_cond;
static pthread_mutex_t mixer_mutex;

msgqueue_t mixerq;

/* Private stuff */
void mixer_msg_loop(void);
void set_volume(int l, int r);
void volume_up(void);
void volume_down(void);
void volume_mute(void);
void display_volume_bar(int v);
void hide_volume_bar(int junk);

int master_volume = 63;	/* Volume level */

int volume_displayed = 0;

bitmap_t volume_control;

/* this should be elsewhere */
typedef struct
{
    __u16 *bitmap;
    short int width;
    short int height;
    short int x;
    short int y;
} backstore_t;

backstore_t volume_backing; /* Backing store for volume control */


/*
 * Initialize mixer control thread
 */
int mixer_init(void)
{
    msgqueue_init(&mixerq);
    pthread_t mixerthread;
  
    pthread_mutex_init(&mixer_mutex, NULL);
    pthread_cond_init(&state_cond, NULL);
    pthread_create(&mixerthread, NULL, (void *) mixer_msg_loop, NULL);

    bitmap_load(&volume_control, "gfx/volume.u16");	
    volume_backing.width = volume_control.width;
    volume_backing.height = volume_control.height;
    volume_backing.bitmap = malloc(volume_control.width * volume_control.height * sizeof(__u16));
    volume_backing.x = 0;
    volume_backing.y = 0;

    if (!volume_backing.bitmap) {
        /* out of memory already, we are in trouble...... */
        DBG_PRINTF("mixer: out-of-memory\n");
        return -1;
    }

    set_volume(master_volume, master_volume);

    return 0;
}


/* Runs forwever waiting for input from mixer queue */
void mixer_msg_loop(void) 
{
    DBG_PRINTF("mixer: processing loop started\n");
    for (;;) {
        msg_t *msg = getq(&mixerq);

        DBG_PRINTF("mixer: msg id %d\n", msg->type);

        switch (msg->type) {
        case MSG_REMOTE_VOLUMEUP:
            volume_up();
            break;

        case MSG_REMOTE_VOLUMEDOWN:
            volume_down();
            break;
        }
        free(msg);
    }
}


/* Raise volume */
void volume_up(void)
{
    master_volume += VOLUME_STEP;
    if (master_volume > VOLUME_MAX) {
        master_volume = VOLUME_MAX;
    }

    set_volume(master_volume, master_volume);
    display_volume_bar(master_volume);
}


/* Lower volume */
void volume_down(void)
{
    master_volume -= VOLUME_STEP;
    if (master_volume < VOLUME_MIN) {
        master_volume = VOLUME_MIN;
    }

    set_volume(master_volume, master_volume);
    display_volume_bar(master_volume);
}


void volume_mute(void)
{
    master_volume = VOLUME_MUTE;
    set_volume(master_volume, master_volume);
}


/* Do the actual work */
void set_volume(int l, int r)
{
    int fd, v, cmd, devs;

    DBG_PRINTF("mixer: set_volume(%d, %d)\n", l, r);
    fd = open(MIXER_DEV, O_WRONLY);

    if (fd == -1) {
        DBG_PRINTF("mixer: Unable to open mixer device\n");
        return;
    }

    ioctl(fd, SOUND_MIXER_DEVMASK, &devs);
    if (devs & SOUND_MASK_PCM) {
        cmd = SOUND_MIXER_WRITE_PCM;
    }
    else if (devs & SOUND_MASK_VOLUME) {
        cmd = SOUND_MIXER_WRITE_VOLUME;
    }
    else {
        close(fd);
        return;
    }
    v = (r << 8) | l;
    ioctl(fd, cmd, &v);
    close(fd);	
}


/* Display the volume bar */
void display_volume_bar(int v)
{
    int x = vid.visx - (volume_control.width + VOLUME_X_OFFSET);
    int y = vid.visy - volume_control.height + VOLUME_Y_OFFSET;

    /* Grab what is in the background first */
    if (volume_displayed == 0) {
        video_copy_u16(&vid,
                    volume_backing.bitmap,
                    volume_backing.width,
                    volume_backing.height,
                    x, y);
        volume_backing.x = x;
        volume_backing.y = y;
    }

    /* Display the volume bar */
    video_blit_u16(&vid,
                    volume_control.bitmap,
                    volume_control.width,
                    volume_control.height,
                    x, y);

    video_set_colour(&vid, 227, 231, 237);
    video_filled_rectangle(&vid , 3 + x + (4 * v), y + 20, 8 + x + (4 * v), y + 37);
    volume_displayed = 1;

    timer_start(TIMER_VOL, 50, 1, &hide_volume_bar);
}


/* Restore backing to video area */
/* We get gere when the timer expires */
void hide_volume_bar(int junk)
{
    video_blit_u16(&vid,
                    volume_backing.bitmap,
                    volume_backing.width,
                    volume_backing.height,
                    volume_backing.x,
                    volume_backing.y);
    volume_displayed = 0;
}

