#include <unistd.h>

#include "screen.h"
#include "video.h"
#include "fonts.h"
#include "remote.h"
#include "msgqueue.h"
#include "play.h"
#include "net.h"
#include "bitmap.h"
#include "httpd.h"
#include "mixer.h"
#include "timer.h"
#include "version.h"

/* Globals: */
vid_t vid;
fonts_t font;
msgqueue_t mainq;
bitmap_t network_icon;
bitmap_t up_icon;
bitmap_t down_icon;
bitmap_t logo;
extern __u16 *area;


void draw_header (const char *str)
{
  video_no_clipping ( &vid );
  video_set_colour ( &vid, 0, 0, 0);
  video_clear_region ( &vid, 0, 0, vid.visx - 100, 48 );
  video_setpen ( &vid, 25, 40 );
  font_write ( &font, &vid, str );
}

int main (void)
{
  pthread_t remote_thread;
  pthread_t httpd_thread;

  /* blah blah blah */
  printf(VERSION_STR
	  ", Copyright (C) 2004 James Pitts "
	  "jim@codeworker.com 2006 Geert Janssen\n"
	  "wmamp comes with ABSOLUTELY NO WARRANTY\n"
	  "This is free software, and you are welcome to redistribute it\n"
	  "under certain conditions of the GPL; "
	  "http://www.gnu.org/copyleft/gpl.html\n\n");

  /* setup video mode */
  if (vid_init(&vid) == -1) {
    return -1;
  }

  /* bits per pixel: */
  vid.var.bits_per_pixel = 16;
  /* set resolution */
  vid.var.xres = HORI_RES;
  vid.var.yres = VERT_RES;
  vid_setvar ( &vid );

  /* clean up please */
  video_clear ( &vid );
  
  /* load the graphics... */
  bitmap_load(&network_icon, "gfx/network.u16");
  bitmap_load(&logo, "gfx/logoblack.u16");

  net_init();

  /* Show logo in bottom left corner: */
  video_blit_u16 ( &vid, 
                   logo.bitmap, 
                   logo.width, 
                   logo.height, 
                   0, 
                   vid.visy - logo.height);
  /*
  video_blit_u16 ( &vid, 
                   up_icon.bitmap, 
                   up_icon.width, 
                   up_icon.height, 
                   vid.visx - up_icon.width - 25, 
                   50 - up_icon.height);

  video_blit_u16 ( &vid, 
                   down_icon.bitmap, 
                   down_icon.width, 
                   down_icon.height, 
                   vid.visx - up_icon.width - down_icon.width - 25, 
                   50 - down_icon.height);
  */

  msgqueue_init ( &mainq );

  /* Fork off audio control thread: */
  if (mp3_init())
    return 0;

  /* Fork off mixer/volume control thread: */
  if (mixer_init())
    return 0;

  if (timer_init(NUM_TIMERS))
    return 0;

  /* Fork off remote control thread: */
  pthread_create(&remote_thread, NULL, (void*) monitor_remote, NULL);

  httpd_init();

  /* Fork off http server thread */
  pthread_create(&httpd_thread, NULL, (void*) httpd_run, NULL);

  if (font_init ( &font ) == -1) 
    return -1;

  if (font_load_face ( &font, "fonts/dustismo.ttf"))
    return -1;

  screen1();

  /* Clean up: */
  pthread_cancel ( remote_thread );
  pthread_join ( remote_thread, NULL);
  fprintf(stderr, "font_close\n");

  font_close ( &font );
  fprintf(stderr, "free bitmaps\n");
  bitmap_free ( &network_icon );
  bitmap_free ( &up_icon );
  bitmap_free ( &down_icon );
  bitmap_free ( &logo );

  fprintf(stderr, "free area\n");
  free ( area ); 

  fprintf(stderr, "Really done\n");

  return 0;
}
