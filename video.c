#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <assert.h>

#include "video.h"

void memset16 (__u16 *dest, __u16 src, unsigned len);

void swap (int *a, int *b)
{
  int t = *a;
  *a = *b;
  *b = t;
}

#if 0
char *fbvisual (__u32 v)
{
  if (v == FB_VISUAL_MONO01) return "FB_VISUAL_MONO01";
  if (v == FB_VISUAL_MONO10) return "FB_VISUAL_MONO10";
  if (v == FB_VISUAL_TRUECOLOR) return "FB_VISUAL_TRUECOLOR";
  if (v == FB_VISUAL_PSEUDOCOLOR) return "FB_VISUAL_PSEUDOCOLOR";
  if (v == FB_VISUAL_DIRECTCOLOR) return "FB_VISUAL_DIRECTCOLOR";
  if (v == FB_VISUAL_STATIC_PSEUDOCOLOR) return "FB_VISUAL_STATIC_PSEUDOCOLOR";

  return "UNKNOWN";
}

void dump_fixed_screen_info( struct fb_fix_screeninfo *f )
{
  char name [17];
  memset (name, 0, sizeof (name));
  memcpy (name, f->id, 16);

  fprintf(stderr, "Id: %s\n", name);
  fprintf(stderr, "Start Mem: %#lx\n", f->smem_start);
  fprintf(stderr, "length Mem: %#x\n", f->smem_len);
  fprintf(stderr, "Type: %#x\n", f->type);
  fprintf(stderr, "Type Aux: %#x\n", f->type_aux);
  fprintf(stderr, "Visual: %#x (%s)\n", f->visual, fbvisual(f->visual) );
  fprintf(stderr, "X Pan Step: %d\n", f->xpanstep);
  fprintf(stderr, "Y Pan Step: %d\n", f->ypanstep);
  fprintf(stderr, "Y Wrap Step: %d\n", f->ywrapstep);
  fprintf(stderr, "Line Length: %d\n", f->line_length);
  fprintf(stderr, "Memory Mapped IO: %#lx\n", f->mmio_start);
  fprintf(stderr, "Memory Mapped IO Len: %d\n", f->mmio_len);
  fprintf(stderr, "Accel : %d\n ", f->accel);
}

void dump_color_map ( struct fb_cmap *cmap ) 
{
  fprintf(stderr, "Start: %d\n", cmap->start);
  fprintf(stderr, "Length: %d\n", cmap->len);

#ifdef DUMPCOLORS
  int i = 0;

  for (i = 0; i < cmap->len; ++i)
  {
    fprintf(stderr, "%d - %#x %#x %#x %#x\n",
	    i, cmap->red[i], cmap->blue[i], cmap->green[i], cmap->transp[i]);
  }
#endif
}

void test_write ( int fb, struct fb_fix_screeninfo *f)
{
  char *p = mmap ( 0, f->smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);

  if (p == (char*)-1) 
  {
    perror ("mmap: ");
    return;
  }

  memset (p, 0, HORI_RES * VERT_RES); /* cls */

  int i = 0;
  for (i = 0; i < VERT_RES; ++i)
  {
    int j = 0;
    for (j = 0; j < HORI_RES; j += 2)
      p[j +      i * HORI_RES] = j;
      p[j + 1 +  i * HORI_RES] = j;
  }
  munmap (p, f->smem_len);
}
#endif

int vid_map (vid_t *vid) 
{
  vid->surf = mmap ( 0, vid->fixed.smem_len, PROT_READ | PROT_WRITE,
		     MAP_SHARED, vid->fb, 0);

  if (vid->surf == (void*)-1) 
  {
    perror ("mmap: ");
    return -1;
  }

  return 0;
}

/*  video_blit_u16_trans
 *
 */
void video_blit_u16_trans ( vid_t *vid, 
			    __u16 *buf,
			    int width,
			    int height,
			    int destx,
			    int desty)
{
  /* TODO:: jp - this can be vastly improved for speed...... */

  __u16 *p = vid->surf + desty * vid->var.xres + destx;
  __u16 *b = buf;

  int i;

  for ( i = 0; i < height; i++) {
    int j;
    __u16 *pp = p;

    for ( j = 0; j < width; j++) {
      if (b[j])
	  *pp = b[j];
      pp++;
    }
    p += vid->var.xres;
    b += width;
  }
}

/*  video_blit_u16
 *
 */
void video_blit_u16 ( vid_t *vid, 
                      __u16 *buf,
                      int width,
                      int height,
                      int destx,
		      int desty)
{
  assert (destx >= 0);
  assert (desty >= 0);

  __u16 *p = vid->surf + desty * vid->var.xres + destx;
  __u16 *b = buf;

  int i;
  for ( i = 0; i < height; i++)
  {
    memcpy (p, b, width * sizeof(__u16));
    p += vid->var.xres;
    b += width;
  }
}

void video_copy_u16 ( vid_t *vid, 
                      __u16 *buf,
                      int width,
                      int height,
                      int x, int y)
{
  assert (width % 2 == 0);
  assert (height % 2 == 0);
  assert (x >= 0);
  assert (y >= 0);

  __u16 *p = vid->surf + y * vid->var.xres + x;

  int i = 0;
  for (i=0; i < height; ++i)
  {
    memcpy (buf + i*width, p, width * sizeof(__u16));
    p += vid->var.xres;
  }
}

/* video_blit
 *
 * used by fonts blitting
 */
void video_blit ( vid_t *vid, 
                  char *buf,
                  int width,
                  int height,
                  int destx, 
                  int desty )
{
  if (buf == NULL) 
    return;

  if (desty < vid->clip_ty)
    desty = vid->clip_ty;
  if (desty + height > vid->clip_by)
    height = vid->clip_by - desty;

  if (destx < vid->clip_tx)
    destx = vid->clip_tx;
  if (destx + width > vid->clip_bx)
    width = vid->clip_bx - destx;

  int i,j;
  int x = 0;
  for ( i = desty; i < desty + height; ++i)
  {
    __u16 *dest = vid->surf + i * HORI_RES;

    for ( j = destx; j < destx + width; ++j)
    {
      unsigned char col = buf[x++];
      if (col != 0) {
	__u16 pixel = MAKECOLOUR(vid, col, col, col);
	/*pixel = 0x123412; */
	dest[j] = pixel;
      }
    }
  }
}

/*
 *  video_clear
 */
void video_clear (const vid_t *vid)
{
  /* clear to black is quick.... */
  memset (vid->surf, 0, vid->fixed.smem_len);
}

void video_clear_region ( const vid_t *vid, 
                          int x, 
                          int y, 
                          int width, 
                          int height)
{
  __u16 *p = vid->surf + y * vid->var.xres + x;

  int i;
  for ( i = 0; i < height; i++)
  {
    memset (p, 0, width * sizeof(__u16));
    p += vid->var.xres;
  }
}

/*
 *  draw_background
 */
void draw_background (vid_t *vid)
{
  int i = 0;
  /*int total = vid->var.yres  * vid->var.xres; */
  int total = vid->visy * vid->visx;
  /*int lastcol = 207; */
  int cola = (180 - rand() % 15) << 8;

  while (i < total) {
    int colb = (180 - rand() % 15) << 8;
    int len = 32 + rand () % 64;
    int inc = (colb - cola) / len;
    /* int len = rand () % 32; */
    int k;
    int col = cola;

    if (i+len > total)
      len = total- i;

    for (k = 0; k < len; k++) {
      col += inc;
      int coli = col >> 8;

      vid->surf[i + k] = MAKECOLOUR(vid, coli, coli, coli);
    }
    i += len;
    cola = colb;
  }
}

/*
 *
 */
int vid_init (vid_t *vid)
{
  memset ( vid, 0, sizeof (vid_t) );

  vid->fb = open ("/dev/fb", O_RDWR);

  if (vid->fb == -1) {
    perror ("Open /dev/fb ");
    return -1;
  }

  int rc = ioctl (vid->fb, FBIOGET_FSCREENINFO, &vid->fixed);
  if (rc == -1) {
    perror ("ioctl: FBIOGET_FSCREENINFO ");
    goto error;
  }

  rc = ioctl (vid->fb, FBIOGET_VSCREENINFO, &vid->var);
  if (rc == -1) {
    perror ("ioctl: FBIOGET_VSCREENINFO ");
    goto error;
  }

  vid_map (vid);

  vid_setvar (vid);

  video_setpen (vid, 0,0);

  vid->visx = HORI_RES - 60;
  vid->visy = VERT_RES;

  return 0;

error:

  vid_close (vid);
  return -1;
}

/*
 *  video_setpen
 */
void video_setpen (vid_t *vid, int x, int y)
{
  vid->penx = x;
  vid->peny = y; 
}

/*
 *  video_set_x
 */
void video_set_x (vid_t *vid, int x )
{
  vid->penx = x;
}

/*  vid_setvar
 *
 */
int vid_setvar (vid_t *vid)
{
  int rc = 0;

  rc = ioctl (vid->fb, FBIOPUT_VSCREENINFO, &vid->var);
  if (rc == -1) {
    perror ("ioctl: FBIOPUT_VSCREENINFO ");
    return -1;
  }

  vid->clip_tx = 0;
  vid->clip_ty = 0;
  vid->clip_bx = vid->var.xres - 1;
  vid->clip_by = vid->var.yres - 1;

  return 0;
}


/*  vid_close
 *
 */
void vid_close (vid_t *vid)
{
  vid_cursor_on (vid);
  close (vid->fb);
}

void vid_cursor_off (vid_t *vid)
{
  struct fb_cursorstate cs;
  memset (&cs, 0, sizeof (cs));

  vid_cursor_state ( vid, &cs );
}

void vid_cursor_on (vid_t *vid)
{
  struct fb_cursorstate cs;
  memset (&cs, 0, sizeof (cs));

  cs.mode = FB_CURSOR_FLASH;

  vid_cursor_state ( vid, &cs );
}

void vid_cursor_state (vid_t *vid, struct fb_cursorstate *cs)
{
#if 1
  return;
#else
  int rc; 
  rc = ioctl (vid->fb, FBIOPUT_CURSORSTATE, cs);
  if (rc == -1) {
    perror ("ioctl: FBIOPUT_CURSORSTATE ");
  }
#endif
}

void vid_wait_vsync (vid_t *vid)
{
  struct fb_vblank vb;
  memset (&vb, 0, sizeof (vb));
  int rc;

  do {
    rc = ioctl (vid->fb, FBIOGET_VBLANK, &vb);
    if (rc == -1) {
      perror ("ioctl: FBIOGET_VBLANK ");
      return ;
    }
  } while ( (vb.flags & FB_VBLANK_VBLANKING) != FB_VBLANK_VBLANKING);
}


/*  video_filled_rectangle
 *
 */
void video_filled_rectangle (vid_t *vid, int x1, int y1, int x2, int y2)
{
  if (y1 > y2)
    swap (&y1, &y2);

  int width = x2 - x1;
  __u16 col = MAKEPENCOLOUR(vid);

  __u16 *p = vid->surf + y1 * vid->var.xres;
  int i;
  for (i = y1; i <= y2; ++i)
  {
    memset16 (p + x1, col, width);
    p += vid->var.xres;
  }
}

/*  video_rectangle
 *
 *  Just the outline...
 *
 */
void video_rectangle (vid_t *vid, int x1, int y1, int x2, int y2)
{
  /* noddy clip */
  /*if (x2 > vid->var.xres) x2 = vid->var.xres; */
  /*if (y2 > vid->var.yres) y2 = vid->var.yres; */

  if (x2 > vid->visx) x2 = vid->visx;
  if (y2 > vid->visy) y2 = vid->visy;
  
  __u16 *b = vid->surf + (y1 * vid->var.xres) + x1;
  __u16 *p = b;

  __u16 col = MAKEPENCOLOUR(vid);

  int width = x2 - x1;

  /* top line */
  memset16 (p, col, width);

  int height = y2 - y1;
  /* lower line */
  p = b + height * vid->var.xres;
  memset16 (p, col, width);

  int i;
  for ( i = y1 * vid->var.xres; i <= y2 * vid->var.xres; i += vid->var.xres)
  {
    *(vid->surf + i + x1) = col;
    *(vid->surf + i + x2) = col;
  }
}

/*
 *   memset16
 */
void memset16 (__u16 *dest, __u16 src, unsigned len)
{
  /* we'll update this soon to 4byte align and blit */
  /* for now, do it the simple way. */

#if 1
  int i;
  for (i = 0; i < len; ++i)
  {
    *dest++ = src;
  }

#else
  if ( ((__u32)dest & 3) != 0)
  {
    /* not 4 byte aligned */
    int todo = 4 - ((__u32)dest & 3);
    fprintf(stderr, "todo %d at start\n", todo);
    int i;
    for (i = 0; i < todo; ++i)
      *dest++ = src;

    len -= todo;
  }

  int todo = ((__u32)dest + len) & 3;
  if (todo)
  {
    fprintf(stderr, "todo %d at end\n", todo);
 /* end isn't on boundary */
    int i;
    for (i = 0; i < todo; ++i)
      *(dest+len-i) = src;

    len -= todo;
  }
  
  int i;
  __u32 l = src << 16 | src;
  __u32 *p = (__u32*) dest;
  for (i = 0; i < len>>1; ++i)
  {
    *p++ = l;
  }
#endif
}


/*  video_set_colour
 *
 */
void video_set_colour (vid_t *vid, int r, int g, int b)
{
  vid->r = r;
  vid->g = g;
  vid->b = b;
}

/*  video_set_clipping
 *
 */
void video_set_clipping (vid_t *vid, int x1, int y1, int x2, int y2)
{
  vid->clip_tx = x1;
  vid->clip_ty = y1;
  vid->clip_bx = x2;
  vid->clip_by = y2;
}


/*  video_no_clipping
 *
 */
void video_no_clipping (vid_t *vid)
{
  vid->clip_tx = 0;
  vid->clip_ty = 0;
  vid->clip_bx = vid->var.xres;
  vid->clip_by = vid->var.yres;
}


void video_v_line ( vid_t *vid, int x, int y, int height)
{
  int y1;
  __u16 *p;

  if (height == 0)
    return;

  if (height < 0) {
    y += height;
    height = -height;
  }

  /*if (height > 50) height = 50; */

  if (y < 0)
    y = 0;

  y1 = y;

  p = vid->surf + y * vid->var.xres + x;
  for (; y1 < y + height; y1++)
  {
    if (p >= 0 && p <= vid->surf + vid->fixed.smem_len)
      if (y1 % 2 == 0)
	*p = MAKEPENCOLOUR ( vid );
    p += vid->var.xres;
  }
}
