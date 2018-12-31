#include "layers.hpp"

using namespace std;

bool sig_get_STL = false;
bool sig_good_STL = false;
bool sig_get_SNL = false;
bool sig_good_SNL = false;
bool sig_get_SDL = false;
bool sig_good_SDL = false;
bool sig_get_SPL = false;
bool sig_good_SPL = false;

bool sig_get_RAL = false;
bool sig_good_RAL = false;
bool sig_get_RTL = false;
bool sig_good_RTL = false;
bool sig_get_RNL = false;
bool sig_good_RNL = false;
bool sig_get_RDL = false;
bool sig_good_RDL = false;

/* Application Layer */
int sender_application_layer(const int pid, const int segment_id) {
    cout << "Sender Application Layer:" << endl;
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    int data_len = 20;
    int tcp_len = 4 * 5;

    char *buf = new char[data_len];
    generate_init_vector(buf, data_len);

    memcpy(shared_memory+kMacLength+kIpLength+tcp_len, buf, data_len);

    write_dat(buf, data_len, kRawSendDat);
    cout << "SAL write dat file OK" << endl;
    print_tcp_data(buf, data_len);

    kill(pid, SIGGOOD);

    delete[] buf;

    while(1) sleep(1);

    return 0;
}

int receiver_application_layer(const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_RAL = false;
    sig_good_RAL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_RAL);
    signal(SIGBAD, signal_handler_RAL);

    // Wait for signals.
    while(!sig_get_RAL) {
        usleep(100);
    }

    if(!sig_good_RAL) {
        cerr << "RTL bad result, RAL end." << endl;
        return FatherBad;
    }

    cout << endl << "Receiver Application Layer:" << endl;

    // do parse work
    char *ptr_header, *ptr_buf;
    int tcp_header_len = 0;
    uint16_t dat_length = get_dat_length(kReadDat);
    HeaderTCP header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[dat_length-kMacLength-kIpLength];
            memcpy(ptr_header, shared_memory+kMacLength+kIpLength, dat_length-kMacLength-kIpLength);
            memcpy(&header, ptr_header, kTcpMaxLength);
            tcp_header_len = get_tcp_data_length(header);
            ptr_buf = new char[dat_length-kMacLength-kIpLength-tcp_header_len];
            memcpy(ptr_buf, shared_memory+kMacLength+kIpLength+tcp_header_len, dat_length-kMacLength-kIpLength-tcp_header_len);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory+kMacLength+kIpLength;
            memcpy(&header, ptr_header, kTcpMaxLength);
            tcp_header_len = get_tcp_data_length(header);
            ptr_buf = shared_memory+kMacLength+kIpLength+tcp_header_len;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            return WrongCopyMode;
    }

    print_tcp_data(ptr_buf, dat_length-kMacLength-kIpLength-tcp_header_len);
    write_dat(ptr_buf, dat_length-kMacLength-kIpLength-tcp_header_len, kRawRecvDat);
    cout << "RAL write dat file OK" << endl;

    if (copy_mode == CopyMode::Copy) {
        delete[] ptr_header;
        delete[] ptr_buf;
    }

    cout << "All done! Press ctrl+C to exit all processes..." << endl;
    return OK;    
}

void signal_handler_RAL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_RAL = true;
            sig_good_RAL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_RAL = true;
            sig_good_RAL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_RAL = false;
            sig_good_RAL = false;
            break;
    }
}

/* Transport Layer */
int sender_transport_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_STL = false;
    sig_good_STL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_STL);
    signal(SIGBAD, signal_handler_STL);

    // Wait for signals.
    while(!sig_get_STL) {
        usleep(100);
    }

    if(!sig_good_STL) {
        cerr << "SAL bad result, STL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Sender Transport Layer:" << endl;

    // do parse work
    int data_len = 20;
    int tcp_len = 4 * 5;

    HeaderTCP header;

    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[data_len+tcp_len];
            memcpy(ptr_header+tcp_len, shared_memory+kMacLength+kIpLength+tcp_len, data_len);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory+kMacLength+kIpLength;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    PseudoHeaderTCP pseudo;
    pseudo.tcp_length = data_len + tcp_len;
    memcpy(ptr_header, &header, tcp_len);
    uint16_t cksum = checksum_TCP_sender(ptr_header, data_len + tcp_len, pseudo);
    header.tcp_check_sum = htons(cksum);
    print_header_TCP(header, cksum);

    memcpy(ptr_header, &header, tcp_len);

    if (copy_mode == CopyMode::Copy) {
        memcpy(shared_memory+kMacLength+kIpLength, ptr_header, tcp_len);
        delete[] ptr_header;
    }
    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK; 
}

int receiver_transport_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_RTL = false;
    sig_good_RTL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_RTL);
    signal(SIGBAD, signal_handler_RTL);

    // Wait for signals.
    while(!sig_get_RTL) {
        usleep(100);
    }

    if(!sig_good_RTL) {
        cerr << "RNL bad result, RTL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Receiver Transport Layer:" << endl;

    // do parse work
    char *ptr_header, *ptr_buf;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[kMaxPacketLen-kMacLength-kIpLength];
            memcpy(ptr_header, shared_memory+kMacLength+kIpLength, kMaxPacketLen-kMacLength-kIpLength);
            ptr_buf = new char[kMaxPacketLen-kMacLength];
            memcpy(ptr_buf, shared_memory+kMacLength, kMaxPacketLen-kMacLength);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory+kMacLength+kIpLength;
            ptr_buf = shared_memory+kMacLength;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    HeaderTCP header;
    memcpy(&header, ptr_header, kTcpMaxLength);

    uint16_t checksum = checksum_TCP(ptr_buf);
    print_header_TCP(header, checksum);

    if (copy_mode == CopyMode::Copy) {
        delete[] ptr_header;
        delete[] ptr_buf;
    }

    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK;    
}

void signal_handler_STL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_STL = true;
            sig_good_STL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_STL = true;
            sig_good_STL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_STL = false;
            sig_good_STL = false;
            break;
    }
}

void signal_handler_RTL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_RTL = true;
            sig_good_RTL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_RTL = true;
            sig_good_RTL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_RTL = false;
            sig_good_RTL = false;
            break;
    }
}

/* Network Layer */
int sender_network_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_SNL = false;
    sig_good_SNL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_SNL);
    signal(SIGBAD, signal_handler_SNL);

    // Wait for signals.
    while(!sig_get_SNL) {
        usleep(100);
    }

    if(!sig_good_SNL) {
        cerr << "STL bad result, SNL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Sender Network Layer:" << endl;

    // do parse work
    int data_len = 20;
    int tcp_len = 4 * 5;

    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[data_len+tcp_len+kIpLength];
            memcpy(ptr_header+kIpLength, shared_memory+kMacLength+kIpLength, data_len+tcp_len);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory+kMacLength;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    HeaderIP header;
    uint16_t cksum = checksum_IP(header);
    header.ip_check_sum = htons(cksum);
    print_header_IP(header);

    memcpy(ptr_header, &header, kIpLength);

    if (copy_mode == CopyMode::Copy) {
        memcpy(shared_memory+kMacLength, ptr_header, kIpLength);
        delete[] ptr_header;
    }

    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK;
}

int receiver_network_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_RNL = false;
    sig_good_RNL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_RNL);
    signal(SIGBAD, signal_handler_RNL);

    // Wait for signals.
    while(!sig_get_RNL) {
        usleep(100);
    }

    if(!sig_good_RNL) {
        cerr << "RDL bad result, RNL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Receiver Network Layer:" << endl;

    // do parse work
    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[kMaxPacketLen-kMacLength];
            memcpy(ptr_header, shared_memory+kMacLength, kMaxPacketLen-kMacLength);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory+kMacLength;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    HeaderIP header;
    memcpy(&header, ptr_header, kIpLength);

    print_header_IP(header);

    if (copy_mode == CopyMode::Copy) {
        delete[] ptr_header;
    }

    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK;   
}

void signal_handler_SNL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_SNL = true;
            sig_good_SNL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_SNL = true;
            sig_good_SNL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_SNL = false;
            sig_good_SNL = false;
            break;
    }
}

void signal_handler_RNL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_RNL = true;
            sig_good_RNL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_RNL = true;
            sig_good_RNL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_RNL = false;
            sig_good_RNL = false;
            break;
    }
}

/* Datalink Layer */
int sender_datalink_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_SDL = false;
    sig_good_SDL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_SDL);
    signal(SIGBAD, signal_handler_SDL);

    // Wait for signals.
    while(!sig_get_SDL) {
        usleep(100);
    }

    if(!sig_good_SDL) {
        cerr << "SNL bad result, SDL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Sender Datalink Layer:" << endl;

    // do parse work
    int data_len = 20;
    int tcp_len = 4 * 5;

    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[data_len+tcp_len+kIpLength+kMacLength];
            memcpy(ptr_header+kMacLength, shared_memory+kMacLength, data_len+tcp_len+kIpLength);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    HeaderMAC header;
    print_header_MAC(header);

    memcpy(ptr_header, &header, kMacLength);

    if (copy_mode == CopyMode::Copy) {
        memcpy(shared_memory, ptr_header, kMacLength);
        delete[] ptr_header;
    }

    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK;
}

int receiver_datalink_layer(const int pid, const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_RDL = false;
    sig_good_RDL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_RDL);
    signal(SIGBAD, signal_handler_RDL);

    // Wait for signals.
    while(!sig_get_RDL) {
        usleep(100);
    }

    if(!sig_good_RDL) {
        cerr << "RPL bad result, RDL end." << endl;
        kill(pid, SIGBAD);
        return FatherBad;
    }

    cout << endl << "Receiver Datalink Layer:" << endl;

    // do parse work
    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[kMaxPacketLen];
            memcpy(ptr_header, shared_memory, kMaxPacketLen);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            kill(pid, SIGBAD);
            return WrongCopyMode;
    }

    HeaderMAC header;
    memcpy(&header, ptr_header, kMacLength);

    print_header_MAC(header);

    if (copy_mode == CopyMode::Copy) {
        delete[] ptr_header;
    }

    kill(pid, SIGGOOD);
    while(1) sleep(1);
    return OK;    
}

void signal_handler_SDL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_SDL = true;
            sig_good_SDL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_SDL = true;
            sig_good_SDL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_SDL = false;
            sig_good_SDL = false;
            break;
    }
}

void signal_handler_RDL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_RDL = true;
            sig_good_RDL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_RDL = true;
            sig_good_RDL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_RDL = false;
            sig_good_RDL = false;
            break;
    }
}

/* Physical Layer */
int sender_physical_layer(const int segment_id, const CopyMode copy_mode) {
    // initialize process
    sig_get_SPL = false;
    sig_good_SPL = false;
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    // bind signal
    signal(SIGGOOD, signal_handler_SPL);
    signal(SIGBAD, signal_handler_SPL);

    // Wait for signals.
    while(!sig_get_SPL) {
        usleep(100);
    }

    if(!sig_good_SPL) {
        cerr << "SDL bad result, SPL end." << endl;
        return FatherBad;
    }

    cout << endl << "Sender Physical Layer:" << endl;

    // do parse work
    int data_len = 20;
    int tcp_len = 4 * 5;

    char *ptr_header;
    switch (copy_mode) {
        case CopyMode::Copy:
            ptr_header = new char[data_len+tcp_len+kIpLength+kMacLength];
            memcpy(ptr_header, shared_memory, data_len+tcp_len+kIpLength+kMacLength);
            break;
        case CopyMode::Shared:
            ptr_header = shared_memory;
            break;
        default:
            cerr << "wrong copy mode" << endl;
            return WrongCopyMode;
    }

    write_dat(ptr_header, data_len+tcp_len+kIpLength+kMacLength, kDat);

    if (copy_mode == CopyMode::Copy) {
        delete[] ptr_header;
    }

    cout << "All done! Press ctrl+C to exit all processes..." << endl;
    return OK;
}

int receiver_physical_layer(const int pid, const int segment_id) {
    cout << "Receiver Physical Layer:" << endl;
    // initialize process
    char *shared_memory = process_init(segment_id);
    if (shared_memory == (char*) -1) { // shared memory allocation error
        kill(pid, SIGBAD);
        GRACEFUL_RETURN("Shared memory attachment\n", SharedMemoryAttachment);
    }

    int len_recv = read_dat(shared_memory, kReadDat);

    if (len_recv == OpenFile) {
        kill(pid, SIGBAD);
        return OpenFile;
    }

    cout << "Bytes read from packet:\t" << len_recv << endl;
    kill(pid, SIGGOOD);

    while(1) sleep(1);

    return OK;    
}

void signal_handler_SPL(int sig) {
    switch (sig) {
        case SIGGOOD:
            //cout << "DEBUG: SIGGOOD detected.\n";
            sig_get_SPL = true;
            sig_good_SPL = true;
            break;
        case SIGBAD:
            cout << "DEBUG: SIGBAD detected.\n";
            sig_get_SPL = true;
            sig_good_SPL = false;
            break;            
        default:
            //cout << "DEBUG: unknown signal detected.\n";
            sig_get_SPL = false;
            sig_good_SPL = false;
            break;
    }
}
