#include "transport_layer.hpp"

using namespace std;

int sender_transport_layer(int lower_layer_pid, int segment_id, CopyMode copy_mode) {
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", ErrorCode::SharedMemoryAttachment);
    }

    // generate random data
    // the length of the payload is 1460
    uint8_t random_data[payload_length];
    generate_init_vector(random_data); // populate with random data

    // write to the payload field of the shared memory segement 
    memcpy(shared_memory[PacketFieldPos::Payload], random_data, rand() % payload_length + 1);
    // write payload to file
    write_bytes_to_file(fn_sender, random_data, PacketFieldPos::PacketEnd);

    // send a signal to transport layer (lower layer) when the shared memory is ready
    kill(lower_layer_pid, SIGUSR1);

    // wait for signal to die
    // TODO(twofyw): find a better way to eliminate the exit delay with 
    // proper stack rewinding
    return ErrorCode::OK;
}

int receiver_transport_layer(int higher_layer_pid, int segment_id, CopyMode copy_mode) {
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", ErrorCode::SharedMemoryAttachment);
    }

    // unpack payload
    uint8_t ptr_payload;
    switch (copy_mode) {
        case CopyMode::copy:
            ptr_payload = new uint8_t[payload_length];
            memcpy(ptr_payload, shared_memory + PacketFieldPos::Payload, payload_length);
            break;
        case CopyMode::shared:
            ptr_payload = shared_memory + PacketFieldPos::Payload;
            break;
        default:
            LOG(Error) << "Wrong copy mode"
            return ErrorCode::WrongCopyMode;
    }

    // write to file
    write_bytes_to_file(fn_receiver, random_data, PacketFieldPos::PacketEnd);


    // release new
    if (copy_mode == CopyMode::copy) {
        delete[] ptr_payload;
    }

    return ErrorCode::OK;
}

void signal_handler(int sig) {
    switch (sig) {
        case SIGUSR1:
            LOG(Info) << "[5] SIGUSR1 ignored.\n";
            break;
        default:
            LOG(Info) << "[5] Unknown signal received.\n";
            break;
    }
}