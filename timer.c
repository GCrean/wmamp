/*
 * Simple Timer package
 * Copyright (c) 2004, Andrew Webster (andrew@fortress.org)
 * 2004-08-20 - Initial revision
 * Distributed under GNU GPL
 * $Id$
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <memory.h>
#include <signal.h>
#include <unistd.h>

#include "timer.h"

/*--------------------------------------------------------------------------*/

static atimer_t *timers;

static int max_timers;

void timer_process(int);

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 *
 * 	Initialize timer system
 *   maxtimers -> how many timers we want in the system
 * Returns: 0= success, -1 = error
 */
int timer_init(int maxtimers)
{
  atimer_t *tp;
  struct itimerval itvalue, itovalue;
  struct sigaction act;
  struct sigaction oldact;

  tp = (atimer_t *) malloc (maxtimers * sizeof (atimer_t));
  if (tp == NULL) 
    return -1;
  max_timers = maxtimers;
  /* Clear all timers */
  memset(tp, 0, maxtimers * sizeof (atimer_t));
  timers = tp;

  /* Setup timer handler */
  memset ( &act, 0, sizeof (act) );
  act.sa_handler = timer_process;
  act.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &act, &oldact);

  /* Setup interval, and away we go... */
  getitimer ( ITIMER_REAL, &itvalue );
  itvalue.it_interval.tv_sec = 0;
  itvalue.it_interval.tv_usec = TIMER_INTERVAL;
  itvalue.it_value.tv_usec = TIMER_INTERVAL;
  if (setitimer ( ITIMER_REAL, &itvalue, &itovalue ) == -1) {
    perror ("setitimer");
  }
  return 0;
}


/*--------------------------------------------------------------------------
 *
 * A timer compare routine
 * Returns -2 (error), -1, 0 or 1 like strcmp
 */
int timer_cmp(int timer1, int timer2)
{
  atimer_t *t1, *t2;

  if (timer1 < 0 || timer1 >= max_timers || timer2 < 0 || timer2 >= max_timers)
    return -2;

  t1 = &timers[timer1];
  t2 = &timers[timer2];

  if (t1->running == 0 || t2->running == 0)
    return -2;

  if (t1->count < t2->count)
    return -1;
  if (t1->count > t2->count)
    return 1;
  return 0;
}

/*--------------------------------------------------------------------------
 * 
 *   Start a timer
 *   timerno -> timer #
 *   delay -> delay in ticks (see timer.h)
 *   iterations -> how many times to fire the callback (0 = infinite)
 *   callback -> function to call on timer expiry
 * Returns: 0=success, -1 = invalid timer, -2 = timer already running
 *
 */
int timer_start(int timerno, long delay, long iterations,
		void (*callback)(int))
{
  atimer_t *tp;	/* hack to make access faster */

  if (timerno < 0 || timerno >= max_timers)
    return -1;

  tp = &timers[timerno];

  tp->start_count = delay;
  tp->reload = delay;
  tp->iterations = iterations;
  tp->iteration_count = iterations;
  tp->callback = callback;
  tp->running = 0;
  tp->start = 1;  /* Off we go... */
  return 0;
}


/*--------------------------------------------------------------------------
 * 
 *   Stop a timer
 *   timerno -> timer #
 * Returns: 0 = success, -1 = invalid timer
 */
int timer_stop(int timerno)
{
  atimer_t *tp;     /* hack to make access faster */

  if (timerno < 0 || timerno >= max_timers)
    return -1;

  tp = &timers[timerno];

  tp->running = 0;
  memset(tp, 0, sizeof(atimer_t));  /* Zap the whole thing */
  return 0;
}


/*--------------------------------------------------------------------------
 * 
 * 	Internal timer processing routine
 *	Called by SIGALRM handler
 */
void timer_process(int dummy)
{
  atimer_t *tp;     /* hack to make access faster */
  int i;

  /* Process all system timers */
  for (i = 0; i < max_timers; ++i) {
    tp = &timers[i];

#if 0
    fprintf(stderr, "%03d %03d %03d %05d %05d %05d %05d\n",
	    i, tp->start, tp->running,
	    tp->count, tp->reload, tp->iterations, tp->iteration_count);
#endif 

    if (tp->running == 0 && tp->start == 0)
      continue;	/* Do nothing, not running. */

    if (tp->start == 1 && tp->running == 1)  /* INVALID STATE, skip it */
      continue;

    if (tp->running == 0 && tp->start == 1) {   /* We want to start it */
      tp->count = tp->start_count;
      tp->start = 0;
      tp->running = 1;
      continue;
    }

    /* If we get here, must be running == 1 and start == 0 */
    if (tp->count > 0) 
      tp->count--;   /* Fall thru */

    /* A timer has expired... */
    if (tp->count == 0) {
      /* If we get here, running == 1, start == 0, count == 0 */

      if (tp->reload == 0) {
	tp->running = 0; 	/* Stop the timer */
	if (tp->callback != NULL)
	  (tp->callback)(i);   /* Call the callback fn */
	continue;		/* No reload, so this counter is finished */
      }

      /* These timers get reloaded... */
      /* If we get here, r = 1, s = 0, c = 0, r > 0, i = ?, ic = ? */

      if (tp->iterations == 0) {	/* Infinitely reload */
	tp->count = tp->reload;
	if (tp->callback != NULL)
	  (tp->callback)(i);   	/* Call the callback fn */
	continue;
      }

      /* Timers with Finite number of iterations... */
      /* r = 1, s = 0, c = 0, r > 0, i > 0, ic = ? */
      if (tp->iteration_count > 0)
	tp->iteration_count--; 	/* Fall thru */

      if (tp->iteration_count == 0) 
	tp->running = 0;		/* Stop the timer, no more iterations */
      else
	tp->count = tp->reload;      /* Otherwise reload it */
  
      if (tp->callback != NULL)
	(tp->callback)(i);           /* Call the callback fn */
    }
  }
}
