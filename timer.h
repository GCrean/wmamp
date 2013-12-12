#ifndef TIMERS_H
#define TIMERS_H

#define TIMER_INTERVAL	100000		/* 100ms */

#define NUM_TIMERS	4


#define TIMER_NET	1		/* Reserved for network icon */
#define TIMER_VOL	2		/* For volume bar */
#define TIMER_SHUFFLE	3

typedef struct timer_struc {
  long count;                   /* Amount of time remaining */
  long start_count;		/* Start value */
  long reload;			/* Reload value */
  long iteration_count;		/* # of iterations */
  long iterations;		/* how many times to reload */
  void (*callback)(int);             /* Function to callback */
  char start;
  char running;			/* Running flag */
} atimer_t;


int timer_init(int maxtimers);
int timer_start(int timerno, long delay, long iterations, void (*callback)(int)) ;
int timer_stop(int timerno);

#endif /* __TIMERS_H */
