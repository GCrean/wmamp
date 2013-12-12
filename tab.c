/*
 * tab.h - draw nice tabs at the top of the screen
 */

//#include <unistd.h>

#include "video.h"
#include "fonts.h"
#include "tab.h"


void draw_active_tab(tab_t tab)
{
    video_no_clipping(&vid);
    video_set_colour(&vid, 55, 65, 75); /* JDH - same colour as highlight in list area */
    video_filled_rectangle(&vid, tab.x1, tab.y1, tab.x2, tab.y2);
    video_set_colour(&vid, 230, 230, 230);
    video_rectangle(&vid, tab.x1, tab.y1, tab.x2, tab.y2);
    video_setpen(&vid, tab.x1 + 5, tab.y1 + 40);
    font_write(&font, &vid, tab.title);
}


void draw_inactive_tab(tab_t tab)
{
    video_no_clipping(&vid);
    video_set_colour(&vid, 20, 20, 40); /* JDH - same colour as background of list area */
    video_filled_rectangle(&vid, tab.x1, tab.y1, tab.x2, tab.y2);
    video_set_colour(&vid, 230, 230, 230);
    video_rectangle(&vid, tab.x1, tab.y1, tab.x2, tab.y2);
    video_setpen(&vid, tab.x1 + 5, tab.y1 + 40);
    font_write(&font, &vid, tab.title);
}


void clear_tab_region(void)
{
    video_no_clipping(&vid);
    video_clear_region(&vid, 0, 0, vid.visx, 48);
}

