/* queue.c - enqueue, dequeue */

#include <xinu.h>

struct qentry	queuetab[NQENT];	/* Table of process queues	*/

/*------------------------------------------------------------------------
 *  enqueue  -  Insert a process at the tail of a queue
 *------------------------------------------------------------------------
 */
pid32	enqueue(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q		/* ID of queue to use		*/
	)
{
	qid16	tail, prev;		/* Tail & previous node indexes	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	tail = queuetail(q);
	prev = queuetab[tail].qprev;

	queuetab[pid].qnext  = tail;	/* Insert just before tail node	*/
	queuetab[pid].qprev  = prev;
	queuetab[prev].qnext = pid;
	queuetab[tail].qprev = pid;
	return pid;
}

/*------------------------------------------------------------------------
 *  dequeue  -  Remove and return the first process on a list
 *------------------------------------------------------------------------
 */
pid32	dequeue(
	  qid16		q		/* ID queue to use		*/
	)
{
	pid32	pid;			/* ID of process removed	*/

	if (isbadqid(q)) {
		return SYSERR;
	} else if (isempty(q)) {
		return EMPTY;
	}

	pid = getfirst(q);
	queuetab[pid].qprev = EMPTY;
	queuetab[pid].qnext = EMPTY;
	return pid;
}

/*------------------------------------------------------------------------
 *  enqueueMsg  -  Enqueue a message in the queue in process table 
 *------------------------------------------------------------------------
 */
bool8 enqueueMsg(umsg32 msg, struct procent *prptr) {
	if(isqueuefull(prptr)) {
		return FALSE;
	}
	else {
		if(prptr->first == -1) {
			prptr->first = 0;
			prptr->last = 0;
		}
		else if(prptr->last == NMSG-1) {
			prptr->last = 0;
		}
		else {
			prptr->last++;
		}
		prptr->prmsgqueue[prptr->last] = msg;
		return TRUE;
	}
}

/*------------------------------------------------------------------------
 *  dequeueMsg  -  dequeue a message from process table and return the deueued message
 *------------------------------------------------------------------------
 */
umsg32 dequeueMsg(struct procent *prptr) {
	umsg32 msg;
	if(isqueueempty(prptr)) {
		return -1;
	}
	else {
		msg = prptr->prmsgqueue[prptr->first];
		if(prptr->first == prptr->last) {
			prptr->first = -1;
			prptr->last = -1;
		}
		else if(prptr->first == NMSG-1) {
			prptr->first = 0;
		}
		else {
			prptr->first++;
		}
	return msg;
	}
}

/*------------------------------------------------------------------------
 *  isqueuefull  -  check if the queue in process table is full
 *------------------------------------------------------------------------
 */
bool8 isqueuefull(struct procent *prptr) {
	if(prptr->first == prptr->last +1 ||(prptr->first == 0 && prptr->last == NMSG-1)) {
		return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------------------
 *  isqueueempty  -  check if the queue in process table is empty
 *------------------------------------------------------------------------
 */
bool8 isqueueempty(struct procent *prptr) {
	if(prptr->first == -1) {
		return TRUE;
	}
	return FALSE;
}

int16 numberOfElements(struct procent *prptr) {
    if(prptr->first == -1 && prptr->last ==-1) {
        return 0;
    }
    int16 i = prptr->first > prptr->last ? (NMSG - prptr->first + prptr->last + 1) : (prptr->last - prptr->first + 1);
    return i;
}
