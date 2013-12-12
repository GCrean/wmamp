#ifndef VIDEO_H
#define VIDEO_H

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <asm/types.h>

/* Packs the 3 RGB color values into 16 bits RGBA:
   5-bit R, 6-bit G, 5-bit B. (No alpha bits in this case)
*/
#define MAKECOLOUR(v,r,g,b) \
    ((((r)>>3) << (v)->var.red.offset)   | \
     (((g)>>2) << (v)->var.green.offset) | \
     (((b)>>3) << (v)->var.blue.offset))

#define MAKEPENCOLOUR(v) MAKECOLOUR(v,(v)->r,(v)->g,(v)->b)

#ifdef I386
/* My IBM T41p has these resolutions.
   Please adopt to whatever your /dev/fb supports.
*/
#define HORI_RES	1400
#define VERT_RES	1050
#else
#define HORI_RES	 640
#define VERT_RES	 480
#endif

typedef struct {
  int fb;
  struct fb_fix_screeninfo fixed;
  struct fb_var_screeninfo var;
  __u16 *surf;
  /* clipping rect */
  int clip_tx, clip_ty, clip_bx, clip_by;
  int penx, peny;
  int r,g,b;
  int visx, visy;
} vid_t;

extern vid_t vid;

void dump_fixed_screen_info( struct fb_fix_screeninfo *f );
void dump_color_map ( struct fb_cmap *cmap ) ;
void test_write ( int fb, struct fb_fix_screeninfo *f);
char *fbvisual (__u32 v);
void vid_close (vid_t *);
int vid_init (vid_t *);
void video_clear (const vid_t *);
int vid_setvar (vid_t *vid);
void draw_background (vid_t *);
void vid_cursor_on (vid_t *vid);
void vid_cursor_off (vid_t *vid);
void vid_cursor_state ( vid_t *vid, struct fb_cursorstate *cs);
int vid_map (vid_t *vid);
void video_blit ( vid_t *vid, char *buf, int width, int height, int x, int y);
void video_blit_u16 ( vid_t *, __u16 *, int , int , int x, int y);
void video_blit_u16_trans ( vid_t *, __u16 *, int , int , int x, int y);
void video_copy_u16 ( vid_t *vid, __u16 *buf, int width, int height, int x, int y);


void video_setpen (vid_t *vid, int x, int y);
void video_set_x (vid_t *vid, int x );
void vid_wait_vsync (vid_t *vid);
void video_filled_rectangle (vid_t *vid, int x1, int , int x2, int y2);
void video_rectangle (vid_t *vid, int x1, int , int x2, int y2);
void video_set_colour (vid_t *vid, int r, int g, int b);

void video_set_clipping ( vid_t *vid, 
                          int x1, 
                          int y1, 
                          int x2, 
                          int y2 );

void video_no_clipping ( vid_t *vid );

void video_v_line ( vid_t *vid, 
                    int x, 
                    int y, 
                    int height );

void video_clear_region ( const vid_t *vid,
                          int x,
                          int y,
                          int width,
                          int height );



#endif
