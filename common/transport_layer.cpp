#include "transport_layer.hpp"

using namespace std;

int sender_transport_layer(int lower_layer_pid, int segment_id, CopyMode copy_mode) {
    former_not_ready = true;
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", ErrorCode::SharedMemoryAttachment);
    }

    // wait for SIGUSR1
    while(former_not_ready) {
        usleep(100);
    }

    // generate TCP data
    uint8_t *ptr_tcp_header;
    switch (copy_mode) {
        case CopyMode::copy:
            ptr_tcp_header = new uint8_t[kTcpHeaderCopyLength];
            memcpy(ptr_tcp_header[kTcpHeaderLength], shared_memory[PacketFieldPos::Payload], kPayloadLength);
            break;
        case CopyMode::shared:
            ptr_tcp_header = shared_memory + PacketFieldPos::TcpHeader
            break;
        default:
            LOG(Error) << "Wrong copy mode"
            return ErrorCode::WrongCopyMode;
    }
    generate_tcp_header(ptr_tcp_header); // populate with tcp header

    // write to the TCP header field of the shared memory segment
    memcpy(shared_memory[PacketFieldPos::TcpHeader], ptr_tcp_header, kTcpHeaderLength);

    // send a signal to network layer (lower layer) when the shared memory is ready
    kill(lower_layer_pid, SIGUSR1);

    // release new
    if (copy_mode == CopyMode::copy) {
        delete[] ptr_tcp_header;
    }

    return ErrorCode::OK;
}

int receiver_transport_layer(int higher_layer_pid, int segment_id, CopyMode copy_mode) {
    former_not_ready = true;
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", ErrorCode::SharedMemoryAttachment);
    }

    // wait for SIGUSR1
    while(former_not_ready) {
        usleep(100);
    }

    // parse TCP data
    uint8_t *ptr_tcp_header;
    switch (copy_mode) {
        case CopyMode::copy:
            ptr_tcp_header = new uint8_t[kTcpHeaderCopyLength];
            memcpy(ptr_tcp_header, shared_memory[PacketFieldPos::TcpHeader], kTcpHeaderCopyLength);
            break;
        case CopyMode::shared:
            ptr_tcp_header = shared_memory + PacketFieldPos::TcpHeader
            break;
        default:
            LOG(Error) << "Wrong copy mode"
            return ErrorCode::WrongCopyMode;
    }
    parse_tcp_header(ptr_tcp_header); // populate with tcp header
    
    // send a signal to application layer (upper layer) when the shared memory is ready
    kill(getppid(), SIGUSR1);

    // release new
    if (copy_mode == CopyMode::copy) {
        delete[] ptr_tcp_header;
    }

    return ErrorCode::OK;
}

void signal_handler(int sig) {
    switch (sig) {
        case SIGUSR1:
            former_not_ready = false;
            LOG(Info) << "[4] SIGUSR1 get, former layer is ready.\n";
            break;
        default:
            LOG(Error) << "[4] Unknown signal received.\n";
            break;
    }
}

void generate_tcp_header(uint8_t *ptr_tcp_header) {
    // Initialize TCP pseudo header.
    PseudoHeaderTcp pseudo = {kSourceAddr, kDestAddr, 0, Protocol::TCP, 0};

    // Initialize TCP header.
    HeaderTcp header = {kTcpSourcePort, kTcpDestPort, 0, 0, 0x5000, 0, 0, 0};

    // Calculate check_sum.
    header.check_sum = check_sum_calc(ptr_tcp_header);
}

void parse_tcp_header(uint8_t *ptr_tcp_header);

void tcp_header_printer(const HeaderTcp header) {
    cout << endl << "TCP header info: " << endl;
    cout << "source port: \t" << header.source_port << endl;
    cout << "destination port: \t" << header.dest_port << endl;
    cout << "seq number: \t" << header.seq << endl;
    cout << "ack number: \t" << header.ack << endl;
    cout << "data offset and flags: \t" << hex << header.data_offset_and_flags << endl;
    cout << "window size:\t" << dec << header.window_size << endl;
    cout << "check sum:\t" << header.check_sum << endl;
    cout << "urgent pointer:\t" << header.urgent_pointer << endl;
}