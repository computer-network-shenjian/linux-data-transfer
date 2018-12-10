#include "sender.hpp"
#include "../common/types.hpp"
#include "../common/utils.hpp"

int main(int argc, char *argv[]) {
    std::ofstream log_stream;
    if(log_init(log_stream, "sender.log", Info) < 0) {
        cout << "[sender] Open log error!" << endl;
        return ErrorCode::OpenFileError;
    }

    int segment_id = allocate_shared_memory(PacketFieldPos::PacketEnd);
    if (segment_id == -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory allocation\n", ErrorCode::SharedMemoryAllocation);
    }

    // continuously fork subprocesses
    pid_t pid = fork();
    if (pid) { // in parent, execute layer 5
        sender_application_layer(pid, segment_id);

        // if the top layer is returned, the shared memory is safe to deallocated
        deallocate_shared_memory(segment_id);
    } else { // in child
        pid = fork();
        if (pid) { // in parent, execute layer 4
            sender_transport_layer(pid, segment_id);
        } else { // in child
            pid = fork();
            if (pid) { // in parent, execute layer 3
                sender_network_layer(pid, segment_id);
            } else { // in child
                pid = fork();
                if (pid) { // in parent, execute layer 2
                    sender_datalink_layer(pid, segment_id);
                } else {
                    pid = fork();
                    if (pid) { // in parent, execute layer 1
                        sender_physical_layer(pid, segment_id);
                    }                    
                }
            }
        }
    }

}

