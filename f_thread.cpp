#include <iostream>
#include <unistd.h> // for sleep()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

struct msg_buffer {
    long msg_type;
    int x;
};

int main() {
    key_t key = ftok("msg_queue", 65);
    int msg_id = msgget(key, 0666 | IPC_CREAT);

    msg_buffer msg_rcv;
    msg_buffer msg_snd;
    while (true) {
        msgrcv(msg_id, &msg_rcv, sizeof(msg_rcv), 1, 0);
        int x = msg_rcv.x;

        int fx = 0;  // set default value to false (0)
        if (x != 0) {
            // return true (1) if x is not zero
            fx = 1;
        }
        msg_snd.msg_type = 2;
        msg_snd.x = fx;
        msgsnd(msg_id, &msg_snd, sizeof(msg_snd), 0);
        // loop indefinitely
        while (true) {
            sleep(10);  // sleep for 10 seconds
        }
    }
    return 0;
}
