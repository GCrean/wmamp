#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "playlist.h"
#include "debug_config.h"

#if DEBUG_PLAYLIST
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
 *  playlist_clone
 */
playlist_info_t *playlist_clone(const playlist_info_t *s)
{
    playlist_info_t *d;

    if (!s) {
        return NULL;
    }

    d = calloc (1, sizeof (*d));

    d->id = s->id; 
    d->pid = s->pid; 

    if (s->name) {
        d->name = strdup(s->name);
    }

    return d;
}


/*
 *  playlist_delete
 */
void playlist_delete(void *p)
{
    playlist_info_t *s = (playlist_info_t *) p;

    if (s == NULL) {
        return;
    }

    if (s->name) {
        free(s->name);
    }

    free(s);
}


/*
 *  playlist_dump
 */
void playlist_dump(playlist_info_t *s)
{
    if (!s) {
        return;
    }

    /* playlist_dump(songinfo); */
    DBG_PRINTF("(%ld) %s - %ld\n", s->id, s->name, s->pid);
}


void playlist_set_name(playlist_info_t *s, const char *name)
{
    if (s->name) {
        DBG_PRINTF("playlist: duplicate name\n");
        free(s->name);
    }

    s->name = strdup(name);
}

