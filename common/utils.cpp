#include "utils.hpp"

using namespace std;

void generate_init_vector(char *buf, const int len) {
    for (int i = 0; i < len; i++) {
        unsigned char c = rand() % 256;
        memcpy(buf+i, &c, 1);
    }
}

char* process_init(int segment_id) {
    // 1. Register death of child when parent dies
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    // 2. Attach shared memory specified by segment_id and return its address
    char *shared_memory = (char*) shmat(segment_id, nullptr, 0);

    // 3. register a proper signal handler
    // SIGUSR1 is all we need for this project
    //signal(SIGUSR1, signal_handler);

    return shared_memory;
}

int allocate_shared_memory(int shared_segment_size) {
    return shmget (IPC_PRIVATE, shared_segment_size,
            IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
}

int deallocate_shared_memory(int segment_id) {
    return shmctl (segment_id, IPC_RMID, 0);
}

bool write_dat(const char *buf, const unsigned int buflen, const char *dat_name) {
    ofstream ofs(dat_name, ofstream::trunc);
    if (!ofs.is_open()) {
        cerr << "write_dat(): open file error" << endl;
        return false;
    }
    for (unsigned int i = 0; i < buflen; i++) {
        ofs << setfill('0') << setw(2);
        unsigned char c = buf[i];
        ofs << hex << (unsigned short int)c << ' ';
        if (i % 16 == 15) {
            ofs << endl;
        }
    }
    ofs.close();
    return true;
}

int read_dat(char *buf, const char *dat_name) {
    ifstream ifs(dat_name);
    if (!ifs.is_open()) {
        cerr << "read_dat(): open file error" << endl;
        return -1;
    }
    
    // b[0] is the first char of hex, b[1] is the second char of hex.
    char b[3] = {0};
    int cnt = 0;
    while (ifs.good()) {
        b[0] = ifs.get();
        if (b[0] == -1) {
            break;
        }
        if (b[0] == '\n') {
            b[0] = ifs.get();
        }
        b[1] = ifs.get();
        ifs.get();
        stringstream ss;
        ss << b;
        unsigned int a;
        ss >> hex >> a;
        unsigned char c = a;
        memcpy(buf + cnt, &c, 1);
        //buf[cnt] = a;
        cnt++;
    }

    ifs.close();
    return cnt;
}

void print_header_MAC(const HeaderMAC header) {
    cout << "MAC header information:" << endl;
    cout << "DstMAC:\t";
    for (int i = 0; i < 5; i++) {
        cout << uppercase << setfill('0') << setw(2);
        cout << hex << (unsigned short int)header.mac_dest_addr[i] << ":";
    }
    cout << setfill('0') << setw(2);
    cout << hex << (unsigned short int)header.mac_dest_addr[5];

    cout << endl << "SrcMAC:\t";
    for (int i = 0; i < 5; i++) {
        cout << setfill('0') << setw(2);
        cout << hex << (unsigned short int)header.mac_src_addr[i] << ":";
    }
    cout << setfill('0') << setw(2);
    cout << hex << (unsigned short int)header.mac_src_addr[5];

    cout << endl << "Type:\t" << hex << setfill('0') << setw(4) << ntohs(header.protocol_type) << "(IP)" << endl;     
}

void print_header_IP(const HeaderIP header) {
    cout << "IP header information:" << endl;
    cout << "ver+len:" << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)header.version_and_header_length << "(IPv4/20)" << endl;
    cout << "tos:\t" << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)header.service_type << endl;
    cout << "iplen:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.packet_length) << '(' << dec << ntohs(header.packet_length) << ')' << endl;
    cout << "id:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.packet_ID) << endl;
    bool df = header.slice_info & htons(0x4000);
    bool mf = header.slice_info & htons(0x2000);
    cout << "flags:\t" << "DF=" << df << ", MF=" << mf << endl;
    uint16_t offset = ntohs(header.slice_info) & 0x1FFF;
    cout << "offset:\t" << uppercase << setfill('0') << setw(4) << hex << offset << '(' << dec << offset << ')' << endl;
    cout << "ttl:\t" << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)header.ttl << '(' << dec << (unsigned short int)header.ttl << ')' << endl;
    cout << "proto:\t" << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)header.type_of_protocol << "(TCP)" << endl;
    cout << "cksum:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.ip_check_sum);

    HeaderIP pseudo = header;
    pseudo.ip_check_sum = 0;
    uint16_t check_sum = checksum_IP(pseudo);
    cout << "(Should be: " << hex << check_sum << ')' << endl;

    cout << "srcip:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(header.source_addr);
    unsigned char ip[4];
    memcpy(ip, &(header.source_addr), 4);
    cout << '(' << dec << (unsigned short int)ip[0] << '.' << (unsigned short int)ip[1] << '.' << (unsigned short int)ip[2] << '.' << (unsigned short int)ip[3] << ')' << endl;
    cout << "dstip:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(header.dest_addr);
    memcpy(ip, &(header.dest_addr), 4);
    cout << '(' << dec << (unsigned short int)ip[0] << '.' << (unsigned short int)ip[1] << '.' << (unsigned short int)ip[2] << '.' << (unsigned short int)ip[3] << ')' << endl;
}

uint16_t checksum_IP(const HeaderIP header) {
    uint32_t sum = 0;
    uint16_t *buf = new uint16_t[kIpLength/2];
    memcpy(buf, &header, kIpLength);
    for (int i = 0; i < kIpLength/2; i++){
        sum += htons(buf[i]);
    }
    
    memcpy(buf, &sum, 4);
    uint16_t result = buf[0] + buf[1];
    delete[] buf;
    return ~result;
}

void print_header_TCP(const HeaderTCP header, const uint16_t check_sum) {
    cout << "TCP header information:" << endl;
    cout << "sport:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.source_port) << '(' << dec << ntohs(header.source_port) << ')' << endl;
    cout << "dport:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.dest_port) << '(' << dec << ntohs(header.dest_port) << ')' << endl;
    cout << "seq:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(header.seq) << '(' << dec << ntohl(header.seq) << "d)" << endl;
    cout << "ack:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(header.ack) << '(' << dec << ntohl(header.ack) << "d)" << endl;
    uint16_t offset = ntohs(header.data_offset_and_flags) >> 12;
    cout << "offset:\t" << offset << '(' << offset*4 << ')' << endl;
    bool *flag = new bool[12];
    for (int i = 0; i < 12; i++) {
        flag[i] = (htons(header.data_offset_and_flags) >> (11-i)) & 1;
    }
    cout << "reserved:";
    for (int i = 0; i < 6; i++) {
        cout << flag[i];
    }
    cout << "(bit)" << endl;
    cout << "code:\t";
    for (int i = 0; i < 6; i++) {
        cout << flag[i+6];
    }
    cout << "(bit, URG=" << flag[6] << "/ACK=" << flag[7] << "/PSH=" << flag[8] << "/RST=" << flag[9] << "/SYN=" << flag[10] << "/FIN=" << flag[11] << ')' << endl;
    cout << "window:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.window_size) << '(' << dec << ntohs(header.window_size) << ')' << endl;
    cout << "cksum:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.tcp_check_sum) << "(Should be: " << uppercase << setfill('0') << setw(4) << check_sum << ')' << endl;

    cout << "urgptr:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(header.urgent_pointer) << endl;

    cout << endl << "TCP additional data:" << endl;
    for (int i = 0; i < 4*(offset-5); i++) {
        cout << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)header.tcp_info[i] << ' ';
        if (i % 16 == 15) {
            cout << endl;
        }
    }
    cout << endl;
}

uint16_t checksum_TCP(const char *buf) {
    uint32_t sum = 0;
    uint16_t tcp_length = get_dat_length(kReadDat)-kMacLength-kIpLength;
    uint16_t *u_buf = new uint16_t[kMaxPacketLen];
    memset(u_buf, 0, kMaxPacketLen);
    
    HeaderIP header_IP;
    memcpy(&header_IP, buf, kIpLength);

    HeaderTCP header_TCP;
    memcpy(&header_TCP, buf+kIpLength, kTcpMaxLength);
    header_TCP.tcp_check_sum = 0;
    uint16_t offset = ntohs(header_TCP.data_offset_and_flags) >> 12;
    //cout << "TCP_offset:\t" << offset << endl;

    PseudoHeaderTCP pseudo;
    memcpy(&(pseudo.source_addr), &(header_IP.source_addr), 4);
    //cout << "pseudo.source_addr:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(pseudo.source_addr) << endl;
    memcpy(&(pseudo.dest_addr), &(header_IP.dest_addr), 4);
    //cout << "pseudo.dest_addr:\t" << uppercase << setfill('0') << setw(8) << hex << ntohl(pseudo.dest_addr) << endl;

    //cout << "pseudo.zeros:\t" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)pseudo.zeros << endl;
    //cout << "pseudo.protocol:\t" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)pseudo.protocol << endl;

    pseudo.tcp_length = htons(tcp_length);
    //cout << "pseudo.tcp_length:\t" << uppercase << setfill('0') << setw(4) << hex << ntohs(pseudo.tcp_length) << endl;

    memcpy(u_buf, &pseudo, kTcpPseudoLength);
    memcpy(u_buf+kTcpPseudoLength, buf+kIpLength, tcp_length);
    memcpy(u_buf+kTcpPseudoLength, &header_TCP, offset*4);

    for (int i = 0; i < int(kMaxPacketLen); i++){
        sum += htons(u_buf[i]);
    }
    
    memcpy(u_buf, &sum, 4);
    uint16_t result = u_buf[0] + u_buf[1];
    //cout << "cksum result:\t" << ~result << endl;
    delete[] u_buf;
    return ~result;
}

int get_dat_length(const char *dat_name) {
    ifstream ifs(dat_name);
    if (!ifs.is_open()) {
        cerr << "get_dat_length(): open file error" << endl;
        return -1;
    }
    
    // b[0] is the first char of hex, b[1] is the second char of hex.
    char b[3] = {0};
    int cnt = 0;
    while (ifs.good()) {
        b[0] = ifs.get();
        if (b[0] == -1) {
            break;
        }
        if (b[0] == '\n') {
            b[0] = ifs.get();
        }
        b[1] = ifs.get();
        ifs.get();
        cnt++;
    }

    ifs.close();
    return cnt;
}

int get_tcp_data_length(const HeaderTCP header) {
    return 4*ntohs(header.data_offset_and_flags) >> 12;
}

void print_tcp_data(const char *buf, const int len) {
    cout << "TCP data:(len=" << len << ')' << endl;
    for (int i = 0; i < len; i++) {
        unsigned char c = buf[i];
        cout << uppercase << setfill('0') << setw(2) << hex << (unsigned short int)c << ' ';
        if (i % 16 == 15) {
            cout << endl;
        }
    }
    cout << endl;
}

uint16_t checksum_TCP_sender(const char *buf, const int buflen, const PseudoHeaderTCP pseudo) {
    uint32_t sum = 0;
    uint16_t *u_buf = new uint16_t[kMaxPacketLen];
    memset(u_buf, 0, kMaxPacketLen*2);

    memcpy(u_buf, &pseudo, kTcpPseudoLength);
    memcpy(u_buf+kTcpPseudoLength/2, buf, buflen);

    for (int i = 0; i < int(kMaxPacketLen); i++){
        sum += htons(u_buf[i]);
    }
    
    memcpy(u_buf, &sum, 4);
    uint16_t result = u_buf[0] + u_buf[1];
    delete[] u_buf;
    return ~result;
}