/* PPM format [see http://netpbm.sourceforge.net/doc/ppm.html]

   Our restricted interpretation:
   <Magic number: P6>NL
   <Width: decimal number>Space<Height: decimal number>NL
   <Max Color: decimal number, expect 255>NL
   <Height rows from top to bottom of width pixels from left to right,
    each pixel a triplet of RGB bytes in that order> 

   PPM to RAW: strip of first 3 lines.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Packs the 3 RGB color values into 16 bits RGBA:
   5-bit R, 6-bit G, 5-bit B. (No alpha bits in this case)
*/
#define MAKECOLOUR(r,g,b) (((r>>3)<<11) | ((g>>2)<<5) | (b>>3))

typedef short int __u16;

int main (int argc, char *argv[])
{
  short int W;
  short int H;
  FILE *in_fp;
  int in, out;
  unsigned char r,g,b;
  int maxcolorval;
  int n, p;

  if (argc != 3) {
    printf ("Converts 24-bit PPM to 16-bit packed RGB with WH header.\n");
    printf ("Usage: ppm2u16 in out\n");
    return 1;
  }

  printf ("ppm2u16 in %s out %s\n", argv[1], argv[2]);

  in = open (argv[1], O_RDONLY);
  if (!in == -1) {
    printf ("Couldn't open input file\n");
    return 1;
  }

  out = open (argv[2], O_WRONLY|O_CREAT, 0666);
  if (!out == -1) {
    printf ("Couldn't open output file\n");
    return 1;
  }

  in_fp = fdopen(in, "r");

  fscanf(in_fp, "P6\n%n", &n);
  p = n;
  fscanf(in_fp, "%hd %hd\n%n", &W, &H, &n);
  p += n;
  fscanf(in_fp, "%d\n%n", &maxcolorval, &n);
  p += n;

  fprintf(stderr, "width: %d, height: %d pixels\n", W, H);

  /* Write a simple header: */

  /* 16-bit width*/
  write (out, &W, 2);
  /* 16-bit height*/
  write (out, &H, 2);

  /* Sync up in with in_fb: */
  n = lseek(in, p, SEEK_SET);

  while (   read (in, &r, 1) == 1
	 && read (in, &g, 1) == 1
	 && read (in, &b, 1) == 1 ) {
    /* Compact 3x 8-bit RGB into single 16-bit: */
    __u16 col = MAKECOLOUR (r,g,b);
    write (out, &col, 2);
  }

  close (in);
  close (out);
 
  return 0;
}
