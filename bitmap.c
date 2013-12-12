#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>

#include "bitmap.h"
#include "debug_config.h"

#if DEBUG_BITMAP
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
 *   bitmap_load
 */
int bitmap_load (bitmap_t *bitmap, const char *filename)
{
    int size;

    memset(bitmap, 0, sizeof (bitmap_t));

    int fd = open (filename, O_RDONLY);
    if (fd == -1)
    {
        perror ("open ");
        return -1;
    }

    read(fd, &bitmap->width, sizeof(short int));
    read(fd, &bitmap->height, sizeof(short int));
    size = bitmap->width * bitmap->height * sizeof(short int);
    bitmap->bitmap = malloc(size);

    if (bitmap->bitmap == NULL)
    {
        close(fd);
        return -1;
    }

    read(fd, bitmap->bitmap, size);
    close(fd);

    DBG_PRINTF("Loaded %s %d %d\n", filename, bitmap->width, bitmap->height);

    return 0;
}


/*
 *   bitmap_free
 */
void bitmap_free (bitmap_t *bitmap)
{
    if (bitmap->bitmap) {
        free (bitmap->bitmap);
    }
    memset(bitmap, 0, sizeof(bitmap_t));
}
