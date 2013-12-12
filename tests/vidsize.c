#include "video.h"


vid_t vid;

int main (void)
{

  if (vid_init (&vid) == -1)
    return -1;

  video_clear ( &vid );

  video_set_colour ( &vid, 0, 255, 0);

  video_rectangle ( &vid, 0, 0, 319, 239 );
  video_rectangle ( &vid, 0, 0, 520-1, 400-1 );
  //video_rectangle ( &vid, 0, 0, 600-1, 420-1 );
  //video_rectangle ( &vid, 0, 0, 610-1, 430-1 );
  //video_rectangle ( &vid, 0, 0, 625-1, 440-1 );
  //video_rectangle ( &vid, 0, 0, 630-1, 440-1 );
  //video_rectangle ( &vid, 0, 0, 625-1, 478-1 );
  //video_rectangle ( &vid, 0, 0, 630-1, 479-1 );
  video_rectangle ( &vid, 0, 0, 625-1, 480-1 );

// 625 X 480

}
