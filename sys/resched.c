/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <sched.h>

// Constants
#define LAMBDA 0.1

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int getnextexpdevprocid(void);
int getnextlinuxprocid(void);

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int nextprocid;                 /* Next process id */
	optr = &proctab[currpid];       /* Current process */

	int schedclass = getschedclass();
	if (schedclass == EXPDISTSCHED) {
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}
	
		nextprocid = getnextexpdevprocid();
		
		/* remove next process from the ready queue */
		dequeue(nextprocid);
		nptr = &proctab[(currpid = nextprocid)];
		nptr->pstate = PRCURR;
	
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif

	} else if (schedclass == LINUXSCHED) {
		// Update goodness score and remaining time quantum counter
		optr->goodness -= optr->counter - preempt;
		optr->counter = preempt;
		if (currpid == NULLPROC || optr->counter <= 0) {
			optr->counter = 0;
			optr->goodness = 0;
		}

		// Check if current process is yielding
		if (optr->pstate != PRCURR) {
			// Check ready queue is empty or not
			if (q[rdyhead].qnext == rdytail) {
				nextprocid = NULLPROC;
				calculateGoodness();
			} else {
				// Select next process based on goodness score
				nextprocid = getnextlinuxprocid();
			}
		} else {
			if (optr->counter == 0) {
				nextprocid = getnextlinuxprocid();
				optr->pstate = PRREADY;
				insert(currpid, rdyhead, optr->goodness);
			} else {
				// Continue current process
				#ifdef RTCLOCK
					preempt = proctab[currpid].counter;
				#endif
				return(OK);
			}
		}

		// Remove next process from ready queue and assign CPU
		dequeue(nextprocid);
		nptr = &proctab[(currpid = nextprocid)];
		nptr->pstate = PRCURR;
		#ifdef	RTCLOCK
			preempt = proctab[currpid].counter;
		#endif
	} else {
		/* no switch needed if current process priority higher than next*/
		if ((optr->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}
		
		/* force context switch */
		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */
		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	
		#ifdef	RTCLOCK
			preempt = QUANTUM;		/* reset preemption counter	*/
		#endif
	}

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

int getnextexpdevprocid(void)
{
	// Generate exponential distribution random number
	int randomvalue = expdev(LAMBDA);

	// Select next process id based on exponential distribution scheduling
	int nextprocid = rdyhead;
	do {
		nextprocid = q[nextprocid].qnext;
	} while (nextprocid != q[rdytail].qprev && randomvalue >= q[nextprocid].qkey);

	if (nextprocid == rdytail) {
		return NULLPROC;
	}

	return nextprocid;
}

int getnextlinuxprocid()
{
	// Select the process with highest goodness score
	int nextprocid;
	if (q[rdyhead].qnext == rdytail) {
		return NULLPROC;
	} else if (q[q[rdytail].qprev].qkey == 0) {
		calculateGoodness();
	}

	return q[rdytail].qprev;
}

/* Calaculate goodness score for all processes */
void calculateGoodness()
{
	int i;
	for (i = 0; i < NPROC; i++) {
		if (proctab[i].pstate != PRFREE) {
			proctab[i].counter = (proctab[i].counter / 2) + proctab[i].pprio;
			proctab[i].goodness = proctab[i].counter + proctab[i].pprio;
		}
	}

	// Reset ready queue
	int curr = q[rdyhead].qnext;
	while (curr != rdytail) {
		int next = q[curr].qnext;
		if (q[curr].qkey == 0 && curr != NULLPROC) {
			dequeue(curr);
			insert(curr, rdyhead, proctab[curr].goodness);
		}
		
		curr = next;
	}
}