
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "discover.h"
#include "msgqueue.h"
#include "msgtypes.h"
#include "debug_config.h"

#if DEBUG_DISCOVER
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

extern msgqueue_t mainq;


/* create multicast 224.0.0.251:5353 socket */
static int msock()
{
    int s, flag = 1, ittl = 255;
    struct sockaddr_in in;
    struct ip_mreq mc;
    char ttl = 255;

    memset(&in, 0, sizeof(in));
    in.sin_family = AF_INET;
    in.sin_port = htons(5353);
    in.sin_addr.s_addr = 0;

    if((s = socket(AF_INET,SOCK_DGRAM,0)) < 0) return 0;
#ifdef SO_REUSEPORT
    setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (char*)&flag, sizeof(flag));
#endif
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));
    if(bind(s,(struct sockaddr*)&in,sizeof(in))) { close(s); return 0; }

    mc.imr_multiaddr.s_addr = inet_addr("224.0.0.251");
    mc.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mc, sizeof(mc)); 
    setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
    setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, &ittl, sizeof(ittl));

    flag =  fcntl(s, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(s, F_SETFL, flag);

    return s;
}


/*
 *  QueryCallback
 */
int QueryCallback(mdnsda answer, void *arg, int addrecord)
{
    foundserver_t *found;
    discover_t *this;
    int now;
    int i = 0;

    if (answer->ttl == 0) {
        now = 0;
    }
    else {
        now = answer->ttl - time(0);
    }

    switch (answer->type) {
    case QTYPE_A:
        found = (foundserver_t *)arg;
        while (answer->name[i] != '.') {
            found->name[i] = answer->name[i];
            i++;
        }
        found->name[i] = 0; 
        
        found->addr.s_addr = answer->ip.s_addr;
        DBG_PRINTF("discover: A %s for %d seconds to ip %s\n", answer->name, now, inet_ntoa(found->addr)); /* JDH - mdnsd shouldn't be making endian-ness changes! */
        putq(&mainq, FOUND_SERVER, found);
        return -1;
    case QTYPE_SRV:
        this = (discover_t *)arg;
        found = malloc(sizeof(foundserver_t));
        DBG_PRINTF("discover: SRV %s for %d seconds to %s:%d\n", answer->name, now, answer->rdname, ntohs(answer->srv.port));
        found->port= answer->srv.port;
        mdnsd_query(this->mdnsd_info, answer->rdname, QTYPE_A, QueryCallback, (void *)found);
        return -1;
    default:
        DBG_PRINTF("discover: %d %s for %d seconds with %d data\n", answer->type, answer->name, now, answer->rdlen);
        return 0;   /* we shouldn't be getting anything by QTYPE_A or QTYPE_SRV, but if we do, should we kill the query? */
    }
}


/*
 *  BrowserCallback
 */
int BrowseCallback(mdnsda answer, void *arg, int addrecord)
{
    discover_t *this = (discover_t *)arg;
    removeserver_t *removes;
    int now;
    int i = 0;

    if (answer->ttl == 0) {
        now = 0;
    }
    else {
        now = answer->ttl - time(0);
    }

    if (answer->type != QTYPE_PTR) {
        /* something went wrong - we should only get PTR records    */
        DBG_PRINTF("discover: PTR query returned non-PTR %u\n", answer->type);
        return 0;   /* if we return -1 we can kill the query, but there's no way for to to re-issue (for now) */
    }
    DBG_PRINTF("discover: PTR %s for %d seconds to %s ", answer->name, now, answer->rdname);
    /* If the TTL has hit 0, the service is no longer available. */
    /* The above comment was here before, but no action is taken corresponding to it. */
    if (addrecord) {
        DBG_PRINTF("discover: ADD\n");
        mdnsd_query(this->mdnsd_info, answer->rdname, QTYPE_SRV, QueryCallback, (void *)this);
    }
    else {
        /* record removal */
        DBG_PRINTF("discover: Remove %s\n", answer->rdname);
        removes = malloc(sizeof(removeserver_t));
        while (answer->rdname[i] != '.') {
            removes->name[i] = answer->rdname[i];
            i++;
        }
        removes->name[i] = 0; 
        putq(&mainq, REMOVE_SERVER, removes);
    }
    return 0;
}


int discover_init(discover_t *this)
{
    
    memset(this, 0, sizeof(discover_t));

    this->mdnsd_info = mdnsd_new(1, 1000);

    if ((this->socket = msock()) == 0)
    {
        DBG_PRINTF("discover: mdnsd msock() failed\n");
        return 1;
    }

    /* start a query to search for hosts */
    mdnsd_query(this->mdnsd_info, "_daap._tcp.local.", QTYPE_PTR, BrowseCallback, (void *)this);

    return 0;
}

/*
 *  discover
 */
int discover_run(discover_t *this)
{
    struct message m;
    struct in_addr ip;
    unsigned short int port;
    struct timeval *tv;
    int bsize, ssize = sizeof(struct sockaddr_in);
    unsigned char buf[MAX_PACKET_LEN];
    struct sockaddr_in from, to;
    fd_set fds;
    int s;

    s = this->socket;
    while (!this->stop) {
        tv = mdnsd_sleep(this->mdnsd_info);
        FD_ZERO(&fds);
        FD_SET(s, &fds);
        select(s + 1, &fds, 0, 0, tv);

        if (FD_ISSET(s, &fds))
        {
            while ((bsize = recvfrom(s, buf, MAX_PACKET_LEN, 0, (struct sockaddr*)&from, &ssize)) > 0)
            {
                memset(&m, 0, sizeof(struct message));
                message_parse(&m,buf);
                mdnsd_in(this->mdnsd_info ,&m, &from.sin_addr, from.sin_port);
            }
            if ((bsize < 0) && (errno != EAGAIN)) {
                DBG_PRINTF("discover: can't read from socket %d: %s\n", errno, strerror(errno));
                return 1;
            }
        }
        while (mdnsd_out(this->mdnsd_info, &m, &ip, &port))
        {
            memset(&to, 0, sizeof(to));
            to.sin_family = AF_INET;
            to.sin_port = port;
            to.sin_addr.s_addr = ip.s_addr;
            if (sendto(s, message_packet(&m), message_packet_len(&m), 0, (struct sockaddr *)&to, sizeof(struct sockaddr_in)) != message_packet_len(&m)) {
                DBG_PRINTF("discover: can't write to socket %d: %s\n", errno, strerror(errno));
                return 1;
            }
        }
    }

    return 0;
}


/*
 *  discover_stop
 */
int discover_stop(discover_t *this)
{
    this->stop = 1;

    mdnsd_query(this->mdnsd_info, "_daap._tcp.local.", QTYPE_PTR, NULL, NULL);
    mdnsd_shutdown(this->mdnsd_info);
    mdnsd_free(this->mdnsd_info);

    return 0;
}

