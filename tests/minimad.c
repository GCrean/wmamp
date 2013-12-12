# include <stdio.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <memory.h>
#include <sys/ioctl.h>

#include <mcheck.h>


# include "mad.h"

  int done;
/*
 * This is perhaps the simplest example use of the MAD high-level API.
 * Standard input is mapped into memory via mmap(), then the high-level API
 * is invoked with three callbacks: input, output, and error. The output
 * callback converts MAD's high-resolution PCM samples to 16 bits, then
 * writes them to standard output in little-endian, stereo-interleaved
 * format.
 */

static int decode(unsigned char const *, unsigned long);
int out;
int in;
int main(int argc, char *argv[])
{
  struct stat stat;
  void *fdm;
  int r;

  mtrace();

/*
  if (argc != 1)
    return 1;

  if (fstat(STDIN_FILENO, &stat) == -1 ||
      stat.st_size == 0)
    return 2;

  fdm = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, STDIN_FILENO, 0);
  if (fdm == MAP_FAILED)
    return 3;
*/

  out = open ("/dev/dsp", O_WRONLY);
  r=AFMT_S16_LE;
  ioctl (out, SNDCTL_DSP_SETFMT, &r);
  r=1;
  ioctl (out, SNDCTL_DSP_STEREO, &r);
  r=44100;
  ioctl (out, SNDCTL_DSP_SPEED, &r);

  in = open ("test2.mp3", O_RDONLY);

  done = 0;

  decode(fdm, stat.st_size);

  close (in);
/*
  in = open ("test2.mp3", O_RDONLY);
  done = 0;
  decode(fdm, stat.st_size);
  close (in);
*/

  //if (munmap(fdm, stat.st_size) == -1)
  //  return 4;

  return 0;
}

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
  unsigned char const *start;
  unsigned long length;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static
enum mad_flow input(void *data,
		    struct mad_stream *stream)
{
//  struct buffer *buffer = data;

  if (done) return MAD_FLOW_STOP; 

  printf ("stream.error %x\n", stream->error);

  #define SIZE 64000
  //char *buf = malloc (SIZE+8);
  static char buf1[SIZE + 8];
  static char buf2[SIZE + 8];

  static char *buf = buf1;

  memset (buf, 0, SIZE + 8);

  int Remaining;
  char * ReadStart = buf;
  int ReadSize = SIZE;
  if (stream->error == MAD_ERROR_BUFLEN)
  {

    Remaining = stream->bufend - stream->next_frame;
    memcpy (buf, stream->next_frame, Remaining);
    ReadStart=buf + Remaining;
    ReadSize = SIZE - Remaining;

  }

//  if (!buffer->length)
 //   return MAD_FLOW_STOP;

  if (read (in, ReadStart , ReadSize) != ReadSize)
    done = 1;

  mad_stream_buffer(stream, buf, SIZE);

  if (buf == buf1) buf = buf2; else buf = buf1;


  return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

static inline
signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

static
enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
  unsigned int nchannels, nsamples;
  mad_fixed_t const *left_ch, *right_ch;

  /* pcm->samplerate contains the sampling frequency */

  nchannels = pcm->channels;
  nsamples  = pcm->length;
  left_ch   = pcm->samples[0];
  right_ch  = pcm->samples[1];

  printf ("sample rate %d\n", pcm->samplerate);

#if 0
  write (out, pcm->samples[0], nsamples);
#else
  while (nsamples--) {
    signed int sample;

    /* output sample(s) in 16-bit signed little-endian PCM */

    sample = scale(*left_ch++) << 16;
    sample |= scale(*right_ch++) & 0xFFFF;
    write (out, &sample, sizeof (sample));
   // putchar((sample >> 0) & 0xff);
   // putchar((sample >> 8) & 0xff);

/*
    if (nchannels == 2) {
      sample = scale(*right_ch++);
    write (out, &sample, 2);
    //  putchar((sample >> 0) & 0xff);
    //  putchar((sample >> 8) & 0xff);
    }
*/
  }
#endif

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static
enum mad_flow error(void *data,
		    struct mad_stream *stream,
		    struct mad_frame *frame)
{
  struct buffer *buffer = data;

  fprintf(stderr, "decoding error 0x%04x (%s) at byte offset %u\n",
	  stream->error, mad_stream_errorstr(stream),
	  stream->this_frame - buffer->start);

  /* return MAD_FLOW_BREAK here to stop decoding (and propagate an error) */

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

static
int decode(unsigned char const *start, unsigned long length)
{
  struct buffer buffer;
  struct mad_decoder decoder;
  int result;

  /* initialize our private message structure */

  buffer.start  = start;
  buffer.length = length;

  /* configure input, output, and error functions */

  mad_decoder_init(&decoder, &buffer,
		   input, 0 /* header */, 0 /* filter */, output,
		   error, 0 /* message */);

  /* start decoding */

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  /* release the decoder */

  mad_decoder_finish(&decoder);

  return result;
}
