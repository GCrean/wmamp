#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include "screen.h"
#include "bitmap.h"
#include "timer.h"
#include "debug_config.h"

#if DEBUG_OGG_DECODING
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

extern bitmap_t network_icon;


__u16 *area;
int blinking_icon = 1;


/*
 *   handler
 */
static void handler(int sig)
{
    if (blinking_icon) {
        video_blit_u16(&vid, 
                        area, 
                        network_icon.width, 
                        network_icon.height, 
                        vid.visx - network_icon.width, 
                        vid.visy - network_icon.height);
    }
}


/*
 *   net_init
 */
void net_init(void)
{
    area = malloc(network_icon.width * network_icon.height * sizeof(__u16));

    if (!area) {
        /* out of memory already, we are in trouble...... */
        DBG_PRINTF("net: out-of-memory\n");
        return;
    }

    video_copy_u16(&vid, 
                    area, 
                    network_icon.width, 
                    network_icon.height, 
                    vid.visx - network_icon.width, 
                    vid.visy - network_icon.height);
}


/*
 *  net_recv
 */
int net_recv(int s, void *buf, size_t len, int flags)
{
    int rc;

    video_blit_u16_trans(&vid, 
                        network_icon.bitmap, 
                        network_icon.width, 
                        network_icon.height, 
                        vid.visx - network_icon.width, 
                        vid.visy - network_icon.height);

    rc = recv(s, buf, len, flags);

    timer_start(TIMER_NET, 3, 1, &handler);

    return rc;
}

