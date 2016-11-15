/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/

	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	return msg;
}



/*------------------------------------------------------------------------
 *  Receiving a message from any sender. It causes the calling
    process to wait for a message to be sent (by any process).
    When a message is sent, it is received and returned.
 *------------------------------------------------------------------------
 */



umsg32	receiveMsg(void) {
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return	*/

	mask = disable();
	prptr = &proctab[currpid];

	if(isqueueempty(prptr)) {
	    prptr->prstate = PR_RECV_MQ;
     	resched();		/* Block until message arrives	*/
	}
	msg = dequeueMsg(prptr); /* Retrieve Message */
    restore(mask);
	return msg;
}

/*------------------------------------------------------------------------
 *  Receiving a group (msg_count) of messages (msgs). It
    causes the calling process to wait for msg_count messages
    to be sent. When all messages are in the queue, they are
    then all together immediately received.
 *------------------------------------------------------------------------
 */

syscall receiveMsgs(umsg32 *msgs, uint32 msg_count) {
	intmask	mask;
	struct	procent *prptr;
	uint32 i;
	int16 num;      /* Number of Elements in the queue */

	mask = disable();
	prptr = &proctab[currpid];
	/* if msg_count is greater than the messages in the queue then wait for messages to come */
	while(1){
	    num = numberOfElements(prptr);
        if(msg_count > (uint32)num) {
            prptr->prstate = PR_RECV_MQ;
            resched();                      /*Block until msg_count becomes greater than or equal to numberOfElements*/
        }
        else {
            break;
        }
	}
	for(i = 0; i < msg_count; i++) {
	    msgs[i] = dequeueMsg(prptr);
	}

	restore(mask);
	return OK;
}
