#include <stdlib.h>

#include "video.h"
#include "fonts.h"
#include "bitmap.h"
#include "dialog.h"
#include "msgqueue.h"
#include "msgtypes.h"

#define DIALOG_WIDTH 400
#define DIALOG_HEIGHT 250
#define DIALOG_BUTTON_WIDTH  70
#define DIALOG_BUTTON_HEIGHT 25

/*
 *  dialog_button
 */

void dialog_button (int x, int y, const char *text)
{
  video_set_colour ( &vid, 100, 100, 100 );

  video_filled_rectangle ( &vid, x, y,
			   x + DIALOG_BUTTON_WIDTH,
			   y + DIALOG_BUTTON_HEIGHT );

  video_set_colour ( &vid, 230, 230, 230);

  video_rectangle ( &vid, x - 1, y - 1,
		    x + DIALOG_BUTTON_WIDTH  + 1,
		    y + DIALOG_BUTTON_HEIGHT + 1);

  video_setpen ( &vid, x  + 20, y + DIALOG_BUTTON_HEIGHT - 4 );
  font_write ( &font, &vid, text );
}

/*
 *  dialog_box
 */
int dialog_box (const char *text, int buttons)
{

  bitmap_t info;

  bitmap_load ( &info, "gfx/info.u16");

  __u16 *background = malloc ( (DIALOG_WIDTH+2) * (DIALOG_HEIGHT +2)
			       * sizeof (__u16) );

  int x = (vid.visx - DIALOG_WIDTH)  / 2;
  int y = (vid.visy - DIALOG_HEIGHT) / 2 - 50;

  fprintf(stderr, "%d %d\n", x,y);

  video_copy_u16 ( &vid, 
                   background, 
                   DIALOG_WIDTH  + 2,
                   DIALOG_HEIGHT + 2,
                   x-1,
                   y-1 );

  /* background */
  video_set_colour ( &vid, 30, 30, 50 );
  video_filled_rectangle ( &vid, x, y, x + DIALOG_WIDTH, y + DIALOG_HEIGHT );

  /* outline */
  video_set_colour ( &vid, 220, 220, 200);
  video_rectangle ( &vid, x - 1, y - 1, x + DIALOG_WIDTH + 2,
		    y + DIALOG_HEIGHT + 2);

  /* text window */
  video_rectangle ( &vid, 
                    x + 40 + DIALOG_BUTTON_WIDTH,
                    y + 20,
                    x + DIALOG_WIDTH  - 20,
                    y + DIALOG_HEIGHT - 20);

  /* info icon */
  video_blit_u16_trans ( &vid, 
                         info.bitmap, 
                         info.width, 
                         info.height,
                         x + 20, 
                         y + 20 );

  dialog_button ( x + 20, y + DIALOG_HEIGHT - 20 - DIALOG_BUTTON_HEIGHT, "Ok");

  video_set_clipping ( &vid, 
                       x + 40 + DIALOG_BUTTON_WIDTH,
                       y + 20,
                       x + DIALOG_WIDTH  - 20,
                       y + DIALOG_HEIGHT - 20);

  video_setpen ( &vid, 
                 x + 45 + DIALOG_BUTTON_WIDTH,
                 y + font_y_advance ( &font ) + 20 );

  font_write ( &font, &vid, text );

  int rc =  DIALOG_UNKNOWN;

  while ( rc == DIALOG_UNKNOWN ) {
    msg_t *msg = getq( &mainq );
    /* do the other buttons one day.... */
    switch (msg->type) {
    case MSG_REMOTE_SELECT:
      rc = DIALOG_OK;
      break;

    default:
      break;
    }
    free ( msg );
  }

  video_blit_u16 ( &vid, 
                   background,
                   DIALOG_WIDTH  + 2, 
                   DIALOG_HEIGHT + 2, 
                   x - 1, 
                   y - 1); 

  free ( background );
  bitmap_free ( &info );

  video_no_clipping ( &vid );

  return rc;
}
