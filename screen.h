#ifndef SCREEN_H
#define SCREEN_H

#include "video.h"
#include "fonts.h"
#include "http.h"
#include "msgqueue.h"
#include "commands.h"

/*extern vid_t vid; */
/*extern fonts_t font; */
/*extern msgqueue_t mainq; */

void screen1(void);
void browse_screen(http_t *p, int dbid, long sessionid);
void album_screen(http_t *p, int dbid, long sessionid, const char *artist);
void song_screen(http_t *p, int dbid, long sessionid, song_filter_t sfilter);

void draw_header(const char *);

#endif

