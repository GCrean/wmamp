#ifndef __NET_H
#define __NET_H

void net_init (void);
int net_recv(int s, void *buf, size_t len, int flags);

#endif

