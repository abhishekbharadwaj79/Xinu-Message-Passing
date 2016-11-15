/*  main.c  - main */

#include <xinu.h>
pid32 proc1;
pid32 proc2;
pid32 proc3;
pid32 proc4;
pid32 rproc1;
pid32 rproc2;
pid32 rproc3;
pid32 rproc4;

process sendproc1(pid32 pid, uint32 count) {
	uint32 i = 1;
	uint32 a;
	while(i <= count) {
	    sleep(1);
	    printf("Msg: %u is being sent to pid %d  via sendMsg \n", i,pid);
		a = sendMsg(pid, i);
		if(a==1) {
		    sleep(1);
		    printf("Msg: %u was successfully sent to pid %d via sendMsg  \n", i, pid);
		}
		i++;
	}
	return OK;
}

process sendproc2(pid32 pid, uint32 count) {
    umsg32 arr[5] = {35,36,37,38,39};
    sleep(2);
    printf("Msgs sent via sendMsgs for pid %d are: %u %u %u %u %u \n", pid, arr[0], arr[1], arr[2], arr[3], arr[4]);
	uint32 a = sendMsgs(pid, arr, count);
	sleep(1);
	printf("%u Msgs successfully sent to pid %d\n", a, pid);

	return OK;
}

process sendproc3() {
    umsg32 msg = 1000;
    pid32 arr[2] = {rproc3, rproc4};
    sleep(1);
    printf("Msg: %u is being sent to pids: %d %d via sendnMsg \n", msg, rproc3, rproc4);
	uint32 cnt = sendnMsg(2, arr, msg);
	sleep(1);
	printf("Msg: %u was successfully sent to %u processes via sendnMsg \n", msg, cnt);
	return OK;
}


process recvproc1(uint32 count){
	uint32 i = 0;
	umsg32 msg;
	while(i < count) {
		msg = receiveMsg();
		i++;
		sleep(1);
		printf("Msg received via receiveMsg is: %u \n", msg);
	}
	return OK;
}

process recvproc2(){
    umsg32 msgs[5];
    receiveMsgs(msgs, 5);
    sleep(1);
    printf("Msgs received via receiveMsgs for pid %d are: %u %u %u %u %u \n", rproc2, msgs[0], msgs[1], msgs[2], msgs[3], msgs[4]);
    return OK;
}


process recvproc3(){
    umsg32 msgs[1];
    receiveMsgs(msgs, 1);
    sleep(1);
    printf("Msg received is: %u \n", msgs[0]);
    return OK;
}

process recvproc4(){
    umsg32 msg = receiveMsg();
    sleep(1);
    printf("Msg received is: %u \n", msg);
    return OK;
}

process	main(void) {
    rproc1 = create(recvproc1, 4096, 10, "recvproc1", 1, 5);
	proc1 =	create(sendproc1, 4096, 10, "sendproc1", 2, rproc1, 5);
	rproc2 = create(recvproc2, 4096, 10, "recvproc2", 1, 5);
	proc4 = create(sendproc2, 4096, 10, "sendproc2", 2, rproc2, 5);
	rproc3 = create(recvproc3, 4096, 10, "recvproc3", 0);
	rproc4 = create(recvproc4, 4096, 10, "recvproc4", 0);
	proc3 = create(sendproc3, 4096, 10, "sendproc3",0);

	/*sendMsgs and receiveMsgs*/
	resume(proc4);
    resume(rproc2);

    /*sendMsg and receiveMsg*/
	resume(rproc1);
	resume(proc1);

    /*sendnMsg*/
	resume(proc3);
	resume(rproc3);
	resume(rproc4);

	return OK;
}
