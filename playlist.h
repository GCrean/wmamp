#ifndef PLAYLIST_H_
#define PLAYLIST_H_

typedef struct {
    unsigned long int id;
    unsigned long int pid;
    char *name;
} playlist_info_t;

playlist_info_t *playlist_clone(const playlist_info_t *src);
void playlist_delete(void *s);
void playlist_dump(playlist_info_t *s);
void playlist_set_name(playlist_info_t *s, const char *name);

#endif
