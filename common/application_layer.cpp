#include "application_layer.hpp"

int sender_application_layer(int lower_layer_pid, int segment_id) {
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", ErrorCode::SharedMemoryAttachment);
    }

    // generate random data
    // the length of the payload is 1460
    int payload_length = PacketFieldPos::PacketEnd - PacketFieldPos::Payload;
    uint8_t random_data[payload_length];
    generate_init_vector(random_data); // populate with random data

    // write to the payload field of the shared memory segement 
    memcpy(shared_memory[PacketFieldPos::Payload], random_data, payload_length);

    // send a signal to transport layer (lower layer) when the shared memory is ready
    kill(lower_layer_pid, SIGUSR1);

    // wait for signal to die
    // TODO(twofyw): find a better way to eliminate the exit delay with 
    // proper stack rewinding
    while (!flag_ready_to_exit) sleep(1);
    return ErrorCode::OK;
}

void signal_handler(int sig) {
    switch (sig) {
        case SIGUSR1:
            LOG(Info) << "[5] SIGUSR1 ignored.\n";
            break;
        case SIGUSR2:
            flag_ready_to_exit = true;
            LOG(Info) << "[4] SIGUSR2 received, ready to exit.\n";
            break;
        default:
            LOG(Info) << "[4] Unknown signal received.\n";
            break;
    }
}
