#ifndef DISCOVER_H
#define DISCOVER_H

#include "mdnsd/mdnsd.h"

/*#define RR_CACHE_SIZE 500 */
#define RR_CACHE_SIZE 8

typedef struct {

    int stop;
    int socket;
    mdnsd mdnsd_info;

} discover_t;



extern int discover_init(discover_t *dis);
extern int discover_run(discover_t *dis);
extern int discover_stop(discover_t *dis);

#endif
