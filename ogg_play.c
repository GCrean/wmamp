// man didn't we just hack this to shreds?
// stolen and hacked within an inch of it's life from minimad,
// the example libmad player.
 
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
#include <tremor/ivorbisfile.h>

#include "hexdump.h"
#include "http.h"
#include "play.h"
#include "commands.h"
#include "msgtypes.h"
#include "msgqueue.h"
#include "debug_config.h"

#if DEBUG_OGG_DECODING
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


static OggVorbis_File vf;
static int vorbis_bytes_streamed = 0;
static volatile int seekneeded = -1;
pthread_mutex_t vf_mutex = PTHREAD_MUTEX_INITIALIZER;

static size_t ovcb_read(void *ptr, size_t size, size_t nmemb, void *datasource);
static int ovcb_seek(void *datasource, int64_t offset, int whence);
static int ovcb_close(void *datasource);
static long ovcb_tell(void *datasource);


ov_callbacks vorbis_callbacks = 
{
    ovcb_read,
    ovcb_seek,
    ovcb_close,
    ovcb_tell
};


/*  struct buffer buffer;   */

/*
 *   input
 */
static size_t ovcb_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
/*    struct buffer *buffer = datasource; */
    http_t *p = datasource;
    int got = 0;
#if 0
    if (mp3_check_state(MP3_NEXT))
        return -1;
    
    if (mp3_check_state(MP3_STOP))
        return -1;
#endif
    memset (ptr, 0, size * nmemb);
    got = http_read(p, ptr, size * nmemb);

    vorbis_bytes_streamed += got;
#if 0
    if (got == 0) {
        mp3_set_state(MP3_NEXT);
    }
#endif
    return got;
}


/*
 *    decode_ogg
 */
int decode_ogg(http_t *p)
{
    void *datasource = p;   /* &buffer; */
    int error = 0;
    char pcmout[4096];
    int current_section;
    long bytes = 0;
    int done = 0;

    memset(&vf, 0, sizeof(vf));
  
    pthread_mutex_lock(&vf_mutex);
    error = ov_open_callbacks(datasource, &vf, NULL, 0, vorbis_callbacks);

    if (error) {
        vorbis_callbacks.close_func(datasource);
        pthread_mutex_unlock(&vf_mutex);
        DBG_PRINTF("ogg_play: ov_open_callbacks failed %d\n", error);
        return -1;
    }

    while (!done) {
        switch (mp3_state()) {
        case MP3_PAUSE:
            mp3_wait_state(MP3_EXIT_PAUSE);
            if (mp3_check_state(MP3_PLAY) != MP3_PLAY) {
                continue;   /* JDH - skip to end of while loop to re-parse non-MP3_PLAY state */
            }
            mp3_set_state(MP3_PLAYING);
            break;
        case MP3_NEXT:
            next_item();
            done = 1;
            break;
        case MP3_BACK:
            prev_item();
            done = 1;
            break;
        case MP3_STOP:
            done = 1;
            break;
        default:
            break;
        }

        if (done) {
            break;
        }

        bytes = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
        switch (bytes) {
        case OV_HOLE:
            DBG_PRINTF("ogg_play: OV_HOLE - hole in output\n");
            done = 1;
            break;
        case OV_EBADLINK:
            DBG_PRINTF("ogg_play: OV_EBADLINK - abort...\n");
            done = 1;
            break;
        case 0:
            DBG_PRINTF("ogg_play: read 0 bytes, stream finished\n");
            next_item();
            done = 1;
            break;
        default:
            break;
        }

        if (!done) {
            write(out, pcmout, bytes);
        }
    }

    ov_clear(&vf);
    pthread_mutex_unlock(&vf_mutex);
    return 0;
}


static int ovcb_seek(void *datasource, int64_t offset, int whence)
{
    /* this is a stream */
    /* streams aren't seekable */
    return -1;
}


static int ovcb_close(void *datasource)
{
    /* this is a stream */
//  vorbis_http_close();
    return 0;
}


static long ovcb_tell(void *datasource)
{
    /* this is a stream */
    /* return bytes read */
    return vorbis_bytes_streamed;
}

