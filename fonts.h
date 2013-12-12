#ifndef FONTS_H_
#define FONTS_H_

#include "video.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

typedef struct {
  int width, height;
  int left, top;
  int advancex, advancey;
  unsigned char *buffer;
} glyph_cache_t;

typedef struct {
  FT_Library  library;
  FT_Face     face;      /* handle to face object */

  glyph_cache_t *cache[256];

} fonts_t;

extern fonts_t font;

int font_init (fonts_t *);
int font_close (fonts_t *);
int font_load_face (fonts_t *f, const char *font);
void font_clone (const fonts_t *f, fonts_t *f2);
void font_write (fonts_t *f, vid_t *v, const char *str);
int font_y_advance ( fonts_t *f);
int font_x_advance ( fonts_t *f);

#endif
