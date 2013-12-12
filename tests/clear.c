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

  int fb = open ("/dev/fb", O_RDWR);

  if (fb == -1)
  {
    perror ("open:\n");
    return fb;
  }

  // get fixed screen info
  struct fb_fix_screeninfo fixed;

  int rc = ioctl (fb, FBIOGET_FSCREENINFO, &fixed);
  if (rc == -1)
  {
    perror ("ioctl: FBIOGET_FSCREENINFO\n");
    goto error;
  }

  // get variable screen info
  struct fb_var_screeninfo var;

  rc = ioctl (fb, FBIOGET_VSCREENINFO, &var);
  if (rc == -1)
  {
    perror ("ioctl: FBIOGET_VSCREENINFO\n");
    goto error;
  }

  vid_t vid;
  vid_init (&vid);

  vid.var.bits_per_pixel = 16;
  vid.var.xres = 640;
  vid.var.yres = 480;

  vid_setvar (&vid);
  rc = ioctl (fb, FBIOPUT_VSCREENINFO, &var);
  if (rc == -1)
  {
    perror ("ioctl: FBIOPUT_VSCREENINFO\n");
    goto error;
  }

  draw_background (&vid);

  printf ("any key..\n");
  getchar();

  vid_close (&vid);



error:
  close (fb);
  return 0;
}
