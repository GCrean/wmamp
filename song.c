#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "song.h"
#include "debug_config.h"

#if DEBUG_SONG
/*
 * This macro definition is a GNU extension to the C Standard. It lets us pass
 * a variable number of arguments and does some special magic for those cases
 * when only a string is passed. For a more detailed explanation, see the gcc
 * manual here: http://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Variadic-Macros.html#Variadic-Macros
 */
#define DBG_PRINTF(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define DBG_PRINTF(format, ...)
#endif


/*
 *  song_clone
 */
song_info_t *song_clone(const song_info_t *s)
{
    song_info_t *d;

    if (!s) {
        return NULL;
    }

    d = calloc(1, sizeof (*d));

    d->track = s->track; 
    d->id = s->id; 

    if (s->artist) {
        d->artist = strdup(s->artist);
    }

    if (s->album) {
        d->album = strdup(s->album);
    }

    if (s->genre) {
        d->genre = strdup(s->genre);
    }

    if (s->song) {
        d->song = strdup(s->song);
    }

    return d;
}


/*
 *  song_delete
 */
void song_delete(void *p)
{
    song_info_t *s = (song_info_t *) p;

    if (s == NULL) {
        return;
    }

    if (s->artist) {
        free(s->artist);
    }

    if (s->song) {
        free (s->song);
    }

    if (s->album) {
        free (s->album);
    }

    if (s->genre) {
        free (s->genre);
    }

    free(s);
}


/*
 *  song_dump
 */
void song_dump(song_info_t *s)
{
    if (!s) {
        return;
    }

    DBG_PRINTF("Song %d:\n"
	    "Title (%02d) : %s\n"
	    "Artist     : %s\n"
	    "Album      : %s\n",
	    "Genre      : %s\n",
	    s->id,
	    s->track,
	    s->song,
	    s->artist,
	    s->album,
	    s->genre);
}


void song_set_song(song_info_t *s, const char *song)
{
    if (s->song) {
        free(s->song);
    }

    s->song = strdup(song);
}


void song_set_genre(song_info_t *s, const char *genre)
{
    if (s->genre) {
        free(s->genre);
    }

    s->genre = strdup(genre);
}


void song_set_album(song_info_t *s, const char *album)
{
    if (s->album) {
        free(s->album);
    }

    s->album = strdup(album);
}


void song_set_artist(song_info_t *s, const char *artist)
{
    if (s->artist) {
        free(s->artist);
    }

    s->artist = strdup(artist);
}

