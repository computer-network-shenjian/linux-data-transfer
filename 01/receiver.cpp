#include "receiver.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    /*
    int segment_id = allocate_shared_memory(PacketFieldPos::PacketEnd);
    if (segment_id == -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory allocation\n", ErrorCode::SharedMemoryAllocation);
    }
    */

    // continuously fork subprocesses
    pid_t pid = fork();
    if (pid) { // in parent, execute layer 5
        receiver_application_layer();
    } else { // in child
        pid = fork();
        if (pid) { // in parent, execute layer 4
            receiver_transport_layer();
        } else { // in child
            pid = fork();
            if (pid) { // in parent, execute layer 3
                receiver_network_layer();
            } else { // in child
                pid = fork();
                if (pid) { // in parent, execute layer 2
                    receiver_datalink_layer();
                } else {
                    pid = fork();
                    if (pid) { // in parent, execute layer 1
                        receiver_physical_layer();

                        // when the last layer returns, the shared memory is safe to deallocate
                        //deallocate_shared_memory(segment_id);
                    }                    
                }
            }
        }
    }
    return 0;
}
