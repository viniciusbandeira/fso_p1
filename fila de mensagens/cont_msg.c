#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main(void) {
    int msqid;
    //msqid = msgget(IPC_PRIVATE, (IPC_CREAT | IPC_EXCL | 0600));
    msqid = msgget((key_t)1235, 0600 | IPC_CREAT);

    printf("Using message queue %d\n", msqid);
    struct msqid_ds buf;

    int rc = msgctl(msqid, IPC_STAT, &buf);

    uint msg = (uint)(buf.msg_qnum);
    printf("# messages before post: %u\n", msg);

    printf("Posting message to queue...\n");
    struct msgbuf qmsg;
    qmsg.mtype = 100;
    qmsg.mtext[0] = 'T';

    int res = msgsnd(msqid, &qmsg, 1, MSG_NOERROR);

    rc = msgctl(msqid, IPC_STAT, &buf);

    msg = (uint)(buf.msg_qnum);
    printf("# messages after post: %u\n", msg);

    return 0;
}