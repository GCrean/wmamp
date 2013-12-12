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

#include "mad.h"
#include "hexdump.h"
#include "http.h"
#include "play.h"
#include "commands.h"
#include "msgtypes.h"
#include "msgqueue.h"

#ifdef EQ
#include "nowplaying.h"
#include "video.h"
#endif

#include "debug_config.h"

#if DEBUG_MP3_DECODING
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


#define MP3_SIZE (16 * 1024)

static char *buf1 = NULL;
static char *buf2 = NULL;
static char *buf  = NULL;

/*
 *   input
 */
static enum mad_flow input(void *data, struct mad_stream *stream)
{
    http_t *p = (http_t *) data;
    static int haveread = 0;

    /* Clear MAD_BUFFER_GUARD chunk (last 8 bytes): */
    memset(buf + MP3_SIZE, 0, MAD_BUFFER_GUARD);

    int Remaining;
    char *ReadStart;
    int ReadSize;

    /* MAD_ERROR_BUFLEN (= 0x0001) "input buffer too small (or EOF)" */
    if (stream->error == MAD_ERROR_BUFLEN) {
        /* Some data not decoded; must reissue: */
        Remaining = stream->bufend - stream->next_frame;
        memcpy(buf, stream->next_frame, Remaining);
        ReadStart = buf + Remaining;
        ReadSize  = MP3_SIZE - Remaining;
    }
    else {
        ReadStart = buf;
        ReadSize = MP3_SIZE;
        Remaining = 0;
    }

    int got = http_read(p, ReadStart, ReadSize);

    if (got == 0) {
        /* Reset: */
        buf = buf1;
        haveread = 0;
        next_item();
        return MAD_FLOW_STOP;
    }

    haveread += got;

    mad_stream_buffer(stream, buf, Remaining + got);

    /* Switch between buffers: */
    if (buf == buf1) {
        buf = buf2;
    }
    else {
        buf = buf1;
    }

    return MAD_FLOW_CONTINUE;
}


/*
 *  scale
 */
inline
static int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE) {
        sample = MAD_F_ONE - 1;
    }
    else {
        if (sample < -MAD_F_ONE) {
            sample = -MAD_F_ONE;
        }
    }

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}


/*
 *  output
 */
static enum mad_flow output(void *data,
  		            struct mad_header const *header,
		            struct mad_pcm *pcm)
{
    unsigned int nsamples;
    mad_fixed_t const *left_ch, *right_ch;

    switch (mp3_state()) {
    case MP3_PAUSE:
        mp3_wait_state(MP3_EXIT_PAUSE);
        if (mp3_check_state(MP3_PLAY) != MP3_PLAY) {
            return MAD_FLOW_STOP;   /* JDH - skip to end of while loop to re-parse non-MP3_PLAY state */
        }
        mp3_set_state(MP3_PLAYING);
        break;

    case MP3_NEXT:
        next_item();
        return MAD_FLOW_STOP;

    case MP3_BACK:
        prev_item();
        return MAD_FLOW_STOP;

    case MP3_STOP:
        return MAD_FLOW_STOP;

    default:
        break;
    }

    /* nchannels = pcm->channels;	 assume 2 */
    nsamples = pcm->length;
    left_ch = pcm->samples[0];
    right_ch = pcm->samples[1];

    while (nsamples--) {
        int sample;

        sample  = scale(*left_ch++) << 16;
        sample |= scale(*right_ch++) & 0xFFFF;
        write (out, &sample, sizeof(sample));
    }

    return MAD_FLOW_CONTINUE;
}


#ifdef EQ
/*
 *  this is a really simple bash at an EQ graph
 *  just a quick hack 
 */
enum mad_flow
filter(void *closure, struct mad_stream const *stream,
           struct mad_frame *frame)
{
    int channel, slice, band;
    unsigned long avg = 0;
    int sample;

    /* JDH - if we're not showing the song that's playing don't show the equalizer either */
    if (np_check_state() == 0) {
        return MAD_FLOW_CONTINUE;
    }

    for (channel = 0; channel < 2; channel++) {
        for (band = 1; band < 31; band++) {
            avg = 0;
            for (slice = 0; slice < MAD_NSBSAMPLES(&frame->header); slice++) {
	            mad_fixed_t fixed = frame->sbsample[channel][slice][band];
	            avg += abs(fixed >> (MAD_F_FRACBITS-5));
	            /*avg >> 1;*/
            }
            sample = avg;

            /* Fixed point values cover the range [-8.0, 8.0).
	           Convert this to the int range [-1024, 1023]
	           meaning that [-1.0, 1.0) ==> [-128, 127].
             */
            /*int sample = fixed >> 18; */
            /*DBG_PRINTF("mp3: %3d ", sample); */

            if (sample > 50) {
                sample = 50;
            }
            sample -= 10;
            if (sample < 0) {
                sample = 0;
            }

            video_set_colour (&vid, 255, 255, 255);
            video_v_line (&vid, 250 + band * 3 + (100 * channel), vid.visy - 100, -sample);
            video_set_colour (&vid, 0, 0, 0);
            video_v_line (&vid, 250 + band * 3 + (100 * channel), vid.visy - sample - 100, -50 - sample);
            video_set_colour (&vid, 255, 255, 255);
            video_v_line (&vid, 250 + band * 3 + 1 + (100 * channel), vid.visy - 100, -sample);
            video_set_colour (&vid, 0, 0, 0);
            video_v_line (&vid, 250 + band * 3 + 1 + (100 * channel), vid.visy - sample - 100, -50 - sample);
#if 0
            avg += sample;
#endif
        }
        /*DBG_PRINTF("\n"); */

    return MAD_FLOW_CONTINUE;
}
#endif


/*
 *  error
 */
static enum mad_flow error(void *data, 
                           struct mad_stream *stream,
                           struct mad_frame *frame)
{
    DBG_PRINTF("mp3_play: decoding error 0x%04x (%s)\n", stream->error, mad_stream_errorstr(stream));

    return MAD_FLOW_CONTINUE;
}


/*
 *    decode_mp3
 */
int decode_mp3(http_t *p)
{
    struct mad_decoder decoder;
    int result;

    buf1 = malloc (MP3_SIZE + MAD_BUFFER_GUARD);
    buf2 = malloc (MP3_SIZE + MAD_BUFFER_GUARD);
    buf = buf1;

    mad_decoder_init(&decoder, 
                    p,
                    input, 	/* callback */
                    0, /* header */
#ifdef EQ
                    filter,
#else
                    0,		/* no filter */
#endif
                    output,	/* callback */
                    error,	/* callback */
                    0 /* message */
		  );

    result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    mad_decoder_finish(&decoder);

    free(buf1);
    free(buf2);

    return result;
}

