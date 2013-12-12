#include <string.h>
#include <unistd.h>

#include "video.h"
#include "fonts.h"

int  main (int argc, char *argv[])
{

  vid_t vid;
  vid_init (&vid);

  vid.var.bits_per_pixel = 16;
  vid.var.xres = 640;
  vid.var.yres = 480;

  vid_setvar ( &vid );

  video_clear ( &vid );

  draw_background ( &vid );

  fonts_t f;

  font_init (&f);

  font_load_face (&f, "fonts/coolveti.ttf");

  //font_write (&f, &vid, "Hello world", 100,100);
  video_set_colour (&vid, 20,20,70);
  video_filled_rectangle (&vid, 100,100,vid.var.xres-100,vid.var.yres-300);
  video_set_colour (&vid, 230,230,230);
  video_rectangle (&vid, 100,100,vid.var.xres-100,vid.var.xres-300);
  video_setpen (&vid, 0,100);
  //font_write (&f, &vid, "The Quick Brown Fox Jumped Over The Lazy Dog.!!&^*$123456790");
  video_set_colour (&vid, 80,80,80);
  video_filled_rectangle (&vid, 101,122,vid.var.xres-101,146);
  video_setpen (&vid, 120,120);
  font_write (&f, &vid, "Jims iTunes\n");
  font_write (&f, &vid, "Test iTunes!\n");
  font_write (&f, &vid, "Carolines iTunes!\n");

  int fd = open ("network.u16", O_RDONLY);
  printf ("fd %d\n", fd);

  __u16 buf[62*62];
  int rc = read (fd, buf, 62*62*2);
  printf ("rc %d\n", rc);
  close (fd);

  video_blit_u16_trans ( &vid, buf, 62, 62,640-62,480-62);

  font_close ( &f );


  vid_close (&vid);


  return 0;
}

