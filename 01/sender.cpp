#include "sender.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    std::ofstream log_stream;
    if(log_init(log_stream, fn_sender_log, Info) != ErrorCode::OK) {
        cerr << "[sender] Open log error!" << endl;
        return ErrorCode::LogInit;
    }

    int segment_id = allocate_shared_memory(PacketFieldPos::PacketEnd);
    if (segment_id == -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory allocation\n", ErrorCode::SharedMemoryAllocation);
    }

    // continuously fork subprocesses
    pid_t pid = fork();
    if (pid) { // in parent, execute layer 5
        sender_application_layer(pid, segment_id);
    } else { // in child
        pid = fork();
        if (pid) { // in parent, execute layer 4
            sender_transport_layer(pid, segment_id, copy_mode);
        } else { // in child
            pid = fork();
            if (pid) { // in parent, execute layer 3
                sender_network_layer(pid, segment_id, copy_mode);
            } else { // in child
                pid = fork();
                if (pid) { // in parent, execute layer 2
                    sender_datalink_layer(pid, segment_id, copy_mode);
                } else {
                    pid = fork();
                    if (pid) { // in parent, execute layer 1
                        sender_physical_layer(pid, segment_id, copy_mode);

                        // when the last layer returns, the shared memory is safe to deallocate
                        deallocate_shared_memory(segment_id);
                    }                    
                }
            }
        }
    }

}

