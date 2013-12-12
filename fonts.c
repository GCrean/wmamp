#include "fonts.h"
#include "video.h"

#define GUTTER		5

/*  font_init
 *
 */
int font_init (fonts_t *f)
{
  FT_Error error;

  memset (f, 0, sizeof (fonts_t));
  error = FT_Init_FreeType( &f->library );
  return error ? -1 : 0;
}

/*  font_close
 *
 */
int font_close (fonts_t *f)
{
  int i;

  if (f->face)
    FT_Done_Face ( f->face );

  if (f->library)
    FT_Done_FreeType ( f->library );

  for ( i = 0; i < 256; i++ )
  {
    if (f->cache[i])
    {
      if (f->cache[i]->buffer)
	free(f->cache[i]->buffer);
      free(f->cache[i]);
    }
  }
  return 0;
}


/*  font_load_face
 *
 */
int font_load_face (fonts_t *f, const char *font)
{
  FT_Error error;

  error = FT_New_Face( f->library, font, 0, &f->face );

  if ( error == FT_Err_Unknown_File_Format )
  {
    fprintf(stderr, "the font file could be opened and read, but it appears"
            " that its font format is unsupported\n");
    return error;
  }

  if ( error )
  {
    fprintf(stderr,
	    " ... another error code means that the font file could not"
            " be opened or read, or simply that it is broken...\n");
    return error;
  }

  fprintf(stderr, "Font has %ld face(s)\n", f->face->num_faces);

  error = FT_Set_Pixel_Sizes(
			     f->face,	/* handle to face object */
			     0,		/* pixel_width = height */
			     25 );	/* pixel_height */
  return error;
}

void font_clone (const fonts_t *f, fonts_t *f2)
{
  f2->library = f->library;
}

/*  font_write
 *
 */
void font_write (fonts_t *f, vid_t *v, const char *str)
{
  const char *s = str;
  int retx = v->penx;

  while (*s) {
    unsigned char ch = *s++;
  
    if (f->cache[(int)ch] == 0) {
      int size;
      FT_GlyphSlot slot = f->face->glyph;  /* a small shortcut */
      glyph_cache_t *c;

#ifdef I386
      FT_UInt glyph_index;
      /* retrieve glyph index from character code: */
      glyph_index = FT_Get_Char_Index( f->face, (FT_ULong)ch );

      /* load glyph image into the slot (erases previous one): */
      FT_Load_Glyph( f->face, glyph_index, FT_LOAD_DEFAULT );

      /* convert to an anti-aliased bitmap: */
      FT_Render_Glyph( slot, ft_render_mode_normal );
#else
      FT_Load_Char (f->face, (FT_ULong) ch, FT_LOAD_RENDER);
#endif
      c = malloc (sizeof (glyph_cache_t));
      if (!c) {
	fprintf(stderr, "memory getting low to render character\n");
	return;
      }

      c->width    = slot->bitmap.width;
      c->height   = slot->bitmap.rows;
      c->left     = slot->bitmap_left;
      c->top      = slot->bitmap_top;
      c->advancex = slot->advance.x;
      c->advancey = slot->advance.y;

      /*
	fprintf(stderr, "ch: %c @ %4d,%4d\n", ch, v->penx, v->peny);
	fprintf(stderr, "W:%d H:%d L:%d T:%d AX:%d AY:%d\n",
	c->width, c->height, c->left, c->top,
	c->advancex, c->advancey);
      */

      size = c->width * c->height;
      if ( size ) {
	c->buffer = malloc (size);
	if (!c->buffer) {
	  free (c);
	  fprintf(stderr, "memory getting too low to copy glyph.\n");
	  return;
	}
	memcpy (c->buffer, slot->bitmap.buffer, size); 
      }
      else
	c->buffer = 0;

      f->cache[(int)ch] = c;
    }
   
    if (ch == '\n') {
      v->peny += f->cache[ch]->height + GUTTER;
      v->penx = retx;
      continue;
    }

    video_blit ( v,
		 f->cache[ch]->buffer,
		 f->cache[ch]->width,
		 f->cache[ch]->height,
		 v->penx + f->cache[ch]->left, v->peny - f->cache[ch]->top);

    /* advance expressed as 64/th of pixel. */
    v->penx += f->cache[ch]->advancex >> 6;
    v->peny += f->cache[ch]->advancey >> 6;
  }
}

int font_x_advance ( fonts_t *f)
{
  return f->face->size->metrics.x_ppem;
}

int font_y_advance ( fonts_t *f)
{
  return f->face->size->metrics.y_ppem;
}
