#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include "video.h"

int main (int argc, char *argv[])
{

  vid_t vid;
  vid_init (&vid);

  vid_cursor_off(&vid);

  vid.var.bits_per_pixel = 16;
  vid.var.xres = 640;
  vid.var.yres = 480;

  vid_setvar (&vid);

  draw_background (&vid);

  //printf ("any key..\n");
  //getchar();


  vid_close (&vid);

  return 0;
}
