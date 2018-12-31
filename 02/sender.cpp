#include "sender.hpp"

using namespace std;

int main() {
    int segment_id = allocate_shared_memory(kMaxPacketLen);
    if (segment_id == -1) { // shared memory allocation error
        cerr << "Shared memory allocation" << endl;
        return SharedMemoryAllocation;
    }

    signal(SIGCHLD, SIG_IGN);

    // continuously fork subprocesses
    pid_t pid = fork();
    if (pid) { // in parent, execute layer 5
        sender_application_layer(pid, segment_id);
    } else { // in child
        pid = fork();
        if (pid) { // in parent, execute layer 4
            sender_transport_layer(pid, segment_id, kCopyMode);
        } else { // in child
            pid = fork();
            if (pid) { // in parent, execute layer 3
                sender_network_layer(pid, segment_id, kCopyMode);
            } else { // in child
                pid = fork();
                if (pid) { // in parent, execute layer 2
                    sender_datalink_layer(pid, segment_id, kCopyMode);
                } else { // in son, execute layer 1
                    sender_physical_layer(segment_id, kCopyMode);

                    // when the last layer returns, the shared memory is safe to deallocate
                    deallocate_shared_memory(segment_id);
                }                    
            }
        }
    }
    return 0;
}
