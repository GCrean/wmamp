#ifndef COMMANDS_H
#define COMMANDS_H

#include "http.h"
#include "daap.h"
#include "tree.h"


typedef struct {
    unsigned int *songids;
    unsigned int nr_songs;
    unsigned int curr;
} song_db_t;

song_db_t song_db;


typedef struct {
    unsigned long playlist_id;
    char *artist;
    char *album;
    char *genre;
} song_filter_t;


/* Assume this won't change. */
unsigned long dbid;

int sendLogout(http_t *, unsigned long);

#if 0
tree_t *getArtists(http_t *, unsigned long, unsigned long);
#endif

tree_t *browseArtists(http_t *p, unsigned long sessionid, unsigned long databaseid);
tree_t *browseAlbums(http_t *p, unsigned long sessionid, unsigned long databaseid);
tree_t *browseGenres(http_t *p, unsigned long sessionid, unsigned long databaseid);

int getSong(http_t *p, unsigned long sessionid, unsigned long databaseid, const char *song);

long login(http_t *p);

unsigned long database(http_t *p, unsigned long sessionid);

int getServerInfo(http_t *p);

int getContentCodes(http_t *p);

#if 0
tree_t *getSongsByArtistAlbumPlaylist(http_t *p, unsigned long sessionid,
                                      unsigned long dbid, unsigned long plid,
                                      const char *artist, const char *album,
                                      const char *genre);
#else
tree_t *getSongsByArtistAlbumPlaylist(http_t *p, unsigned long sessionid,
                                      unsigned long dbid, song_filter_t sfilt);
#endif

tree_t *getAlbumsByArtists(http_t *p, unsigned long sessionid, unsigned long dbid, const char*artist);

tree_t *getPlaylists(http_t *p, unsigned long sessionid, unsigned long dbid);
#endif
