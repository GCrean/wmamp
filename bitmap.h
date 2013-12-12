#ifndef __BITMAP_H
#define __BITMAP_H
#include "video.h"


typedef struct
{
    __u16 *bitmap;
    short int width;
    short int height;
} bitmap_t;


int bitmap_load(bitmap_t *bitmap, const char *filename);
void bitmap_free(bitmap_t *bitmap);
#endif
