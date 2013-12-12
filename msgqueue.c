#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include "msgqueue.h"

/**  putq
 *
 *    puts a message to the application message queue
 *    it's the receivers responsibility to free all 
 *    memory associated with it.
 *
 */
int putq (msgqueue_t *m, int type, void *data)
{
  /* Prepare new queue element: */
  msg_node_t *node = malloc (sizeof (msg_node_t));

  if (!node)
    return -1;

  node->msg  = malloc (sizeof (msg_t));
  node->next = NULL;

  if (!node->msg)
    return -1;

  node->msg->type = type;
  node->msg->data = data;

  /* Append to queue: */
  pthread_mutex_lock (&m->msgqueue_mutex);

  if (!m->root)
    m->root = node;
  else
    m->tail->next = node;
  m->tail = node;

  /* Signal that the queue is not empty: */
  pthread_cond_signal ( &m->msgwaiting_cond );
  pthread_mutex_unlock ( &m->msgqueue_mutex );

  return 0; 
}

/*  getq
 *
 */
msg_t *getq (msgqueue_t *m)
{
  msg_node_t *p;
  msg_t *msg;

  pthread_mutex_lock ( &m->msgqueue_mutex );

  if (!m->root)
    /* Block on empty queue. */
    pthread_cond_wait (&m->msgwaiting_cond, &m->msgqueue_mutex);

  /* root shouldn't be null here.. */
  p = m->root;
  m->root = p->next;
  if (!m->root)
    m->tail = NULL;

  pthread_mutex_unlock ( &m->msgqueue_mutex );

  msg = p->msg;
  free( p );

  return msg;
}

/*  cleanmsgqueue
 *
 *   This will delete everything on the message queue.
 *   It will also delete the message data which relies
 *   on the fact that there shouldn't be any dynamically
 *   allocated data on it.
 *
 *   Eventually we should supply a member function so
 *   each allocator gets a call back to clean its
 *   own mess up.
 *
 */
void cleanmsgqueue (msgqueue_t *m)
{
  msg_node_t *p;

  pthread_mutex_lock ( &m->msgqueue_mutex );

  p = m->root;
  while (p) {
    msg_node_t *next = p->next;
    /* What about freeing up the contents p->msg?
       Luckily, only called on mainq whose messages are mostly NULL
       except for REMOVE_SERVER and FOUND_SERVER messages.
       Unlikely to have these still in the queue at this time.
    */
    free (p);
    p = next;
  }
  m->root = m->tail = NULL;

  pthread_mutex_unlock ( &m->msgqueue_mutex );
}

/*  msgqueue_init
 *
 */
void msgqueue_init ( msgqueue_t *m )
{
  pthread_mutex_init ( &m->msgqueue_mutex,  NULL );
  pthread_cond_init  ( &m->msgwaiting_cond, NULL );

  m->root = m->tail = NULL;
}
