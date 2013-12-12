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

  dump_fixed_screen_info( &fixed );

  // get pallette info
  struct fb_cmap cmap;
  memset (&cmap, 0, sizeof (cmap));
  cmap.len = 64;
  cmap.red = (__u16*) malloc (cmap.len * sizeof (__u16));
  cmap.green = (__u16*) malloc (cmap.len * sizeof (__u16));
  cmap.blue = (__u16*) malloc (cmap.len * sizeof (__u16));
  cmap.transp = (__u16*) malloc (cmap.len * sizeof (__u16));

  rc = ioctl (fb, FBIOGETCMAP, &cmap);
  if (rc == -1)
  {
    perror ("ioctl: FBIOGETCMAP\n");
    goto error;
  }

  dump_color_map ( &cmap );


  printf ("Test put pallette\n");
  rc = ioctl (fb, FBIOPUTCMAP, &cmap);
  if (rc == -1)
  {
    perror ("ioctl: FBIOPUTCMAP\n");
    goto error;
  }

  free (cmap.red);
  free (cmap.blue);
  free (cmap.green);
  free (cmap.transp);

  printf ("Test write to vid mem\n");
  test_write (fb, &fixed);


error:
  close (fb);
  return 0;
}
