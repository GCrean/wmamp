#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <pthread.h>

/*typedef enum { MSG_NONE } msg_type; */

typedef struct {
  int type;
  void *data;
} msg_t;

typedef struct msg_node_tag {
  msg_t *msg;
  struct msg_node_tag *next;
} msg_node_t;

typedef struct {
  pthread_mutex_t msgqueue_mutex;
  pthread_cond_t msgwaiting_cond;

  msg_node_t *root;		/* start of message queue */
  msg_node_t *tail;		/* end of message queue */
} msgqueue_t;

extern msgqueue_t mainq;

int putq ( msgqueue_t *m, int type, void *data);
msg_t *getq ( msgqueue_t *m );
void cleanmsgqueue( msgqueue_t *m );
void msgqueue_init ( msgqueue_t *m );

#endif
