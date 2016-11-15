/* send.c - send */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  send  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	send(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	if (prptr->prhasmsg) {
		restore(mask);
		return SYSERR;
	}
	prptr->prmsg = msg;		/* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}









/*------------------------------------------------------------------------
 * Sending message (msg) to process (pid). In case a message
   or a number of messages are already waiting on pid to
   receive them, the new msg will be queued. It will return
   OK on success or SYSERR on error.
 *------------------------------------------------------------------------
 */
syscall	sendMsg(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	/* return error if message can't be enqueued */
	if(isqueuefull(prptr)) {
	    restore(mask);
        return SYSERR;
	}
    enqueueMsg(msg, prptr);
	/* If recipient waiting or in timed-wait make it ready */
	if (prptr->prstate == PR_RECV_MQ) {
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}

/*------------------------------------------------------------------------
 * Sending a group (msg_count) of messages (msgs) to
   process (pid). It will return the number of msgs
   successfully sent or SYSERR on error.
 *------------------------------------------------------------------------
 */

uint32 sendMsgs(
		pid32	pid,		/* ID of recipient process	*/
	  	umsg32 *msgs,		/* Contents of message		*/
		uint32 msg_count
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	uint32 	i;

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	resched_cntl(DEFER_START);
	for(i = 0; i < msg_count; i++) {
	    if(!enqueueMsg(msgs[i], prptr)){
	        break;
	    }
	}
	resched_cntl(DEFER_STOP);

	if (prptr->prstate == PR_RECV_MQ) {
		ready(pid);
	}
	restore(mask);
	return i;
}


/*------------------------------------------------------------------------
 * Sending the message (msg) to the number (pid_count) of
   processes (pids). It will return the number of pids
   successfully sent to or SYSERR on error.
 *------------------------------------------------------------------------
 */

uint32 sendnMsg(
                uint32	pid_count,
                pid32 *pids,
                umsg32	msg
                )
{
    intmask	mask;			/* Saved interrupt mask		*/
	uint32 	i;
	uint32 count = 0;

	mask = disable();
	resched_cntl(DEFER_START);
	for(i = 0; i < pid_count; i++) {
	    if(sendMsg(pids[i],msg) == OK) {
	        count++;
	    }
	}
	resched_cntl(DEFER_STOP);

    restore(mask);

    if(count == 0) {
        return SYSERR;
    }
    return count;
}