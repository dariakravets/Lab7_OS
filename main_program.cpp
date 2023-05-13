#include <iostream>
#include <unistd.h> // for sleep()
#include <sys/ipc.h>
#include <sys/msg.h>

using namespace std;

struct msg_buffer {
    long msg_type;
    int x;
};

int main() {
    key_t key_f = ftok("msg_queue", 65);
    int msg_id_f = msgget(key_f, 0666 | IPC_CREAT);

    key_t key_g = ftok("msg_queue", 66);
    int msg_id_g = msgget(key_g, 0666 | IPC_CREAT);

    int x;
    cout << "Enter a value for x: ";
    cin >> x;

    // send request messages to helper
    msg_buffer msg_snd_f;
    msg_buffer msg_rcv_f;
    msg_snd_f.msg_type = 1;
    msg_snd_f.x = x;
    msgsnd(msg_id_f, &msg_snd_f, sizeof(msg_snd_f), 0);

    msg_buffer msg_snd_g;
    msg_buffer msg_rcv_g;
    msg_snd_g.msg_type = 1;
    msg_snd_g.x = x;
    msgsnd(msg_id_g, &msg_snd_g, sizeof(msg_snd_g), 0);

    bool f_ready = false;
    bool g_ready = false;

    // set up timer
    int timer = 0;
    const int TIMEOUT = 10;  // wait for 10 seconds before prompting user
    while (true) {
        // check if f has responded
        if (!f_ready) {
            if (msgrcv(msg_id_f, &msg_rcv_f, sizeof(msg_rcv_f), 2, IPC_NOWAIT) != -1) {
               int fx = msg_rcv_f.x;
                cout << "f(x) = " << fx << endl;
                f_ready = true;
            }
        }

        // check if g has responded
        if (!g_ready) {
            if (msgrcv(msg_id_g, &msg_rcv_g, sizeof(msg_rcv_g), 2, IPC_NOWAIT) != -1) {
                int gx = msg_rcv_g.x;
                cout << "g(x) = " << gx << endl;
                g_ready = true;
            }
        }

        // check if both f and g are ready
        if (f_ready && g_ready) {
            // perform logical AND operation
            int result = msg_rcv_f.x && msg_rcv_g.x;
            cout << "f(x) && g(x) = " << result << endl;
            break;  // exit loop
        }

        // check if timer has expired
        if (timer >= TIMEOUT) {
            cout << "Waiting for helper program(s) to respond..." << endl;
            cout << "Select an option: 1) continue, 2) stop, 3) continue without asking more" << endl;
            int option;
            cin >> option;
            if (option == 2) {
                cout << "Program stopped by user." << endl;
                return 0;
            }
            timer = 0;  // reset timer
        }

        sleep(1);  // sleep for 1 second
        timer++;  // increment timer
    }

    return 0;
}

