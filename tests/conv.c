#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAKECOLOUR(r,g,b) ( ((r>>3)) | ((g>>2)<< 5 )|((b>>3) << 11))

typedef short int __u16;

int main (int argc, char *argv[])
{
  short int W;
  short int H;
  int fd, out;
  unsigned char r,g,b;

  if (argc != 5) {
    printf ("Converts 8-bit RGB to 16-bit packed format with WH header.\n");
    printf ("Usage: conv in out W H\n");
    return -1;
  }

  printf ("in %s out %s\n", argv[1], argv[2]);
  W = atoi (argv[3]);
  H = atoi (argv[4]);
  printf ("X %d Y %d\n", W, H);

  fd  = open (argv[1], O_RDONLY);

  if (fd == -1) {
    printf ("Couldn't open input file\n");
    return -1;
  }

  out = open (argv[2], O_WRONLY|O_CREAT, 0666);

  if (out == -1) {
    printf ("Couldn't open output file\n");
    return -1;
  }

  /* Write a simple header: */

  /* 16-bit width*/
  write (out, &W, sizeof (W));
  /* 16-bit height*/
  write (out, &H, sizeof (H));

  while (   read (fd, &b, 1) == 1
	 && read (fd, &g, 1) == 1
	 && read (fd, &r, 1) == 1 ) {
    /* Compact 3x 8-bit RGB into single 16-bit: */
    __u16 col = MAKECOLOUR (r,g,b);
    write (out, &col, 2);
  }

  close (out);
  close (fd);
 
  return 0;
}
