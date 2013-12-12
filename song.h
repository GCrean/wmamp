#ifndef _SONG_H_
#define _SONG_H_

typedef struct {
    char *song;
    char *artist;
    char *album;
    char *genre;
    unsigned int id; /* itunes id (really unsigned?) */
    unsigned char track;
    unsigned char compilation; /* is it part of a compilation */
} song_info_t;

song_info_t *song_clone(const song_info_t *src);
void song_delete(void *s);
void song_dump(song_info_t *s);
void song_set_artist(song_info_t *s, const char *artist);
void song_set_album(song_info_t *s, const char *album);
void song_set_genre(song_info_t *s, const char *genre);
void song_set_song(song_info_t *s, const char *song);

#endif
