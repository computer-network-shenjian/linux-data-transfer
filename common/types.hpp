#include <stdint.h>

// Note: Enumerators should be named like constants. See Google C++ Style Guide for detail
enum class CopyMode {
    Copy,
    Shared
};

const unsigned int kMaxPacketLen = 1514;

// status codes
enum StatusCode {
    OK = 0,
    OpenFile = -1,
    SharedMemoryAllocation = -2,
    SharedMemoryAttachment = -3,
    LogInit = -4,
    WrongCopyMode = -5,
    WrongHeaderType = -6,
    FatherBad = -7
};

// MAC header, 14 bytes(fixed).
const int kMacLength = 14;
struct HeaderMAC {
    unsigned char mac_dest_addr[6] = {0xa4, 0x5e, 0x60, 0xba, 0xd3, 0x91};   // 6 bytes
    unsigned char mac_src_addr[6] = {0xc4, 0xca, 0xd9, 0xdf, 0x3f, 0x00};    // 6 bytes
    uint16_t protocol_type = htons(0x0800);     // 2 bytes
};

// IP header, 20 bytes to 60 bytes, SJ fixes it to be 20 bytes.
const int kIpLength = 20;
struct HeaderIP {
    // [version, 4 bit][header length, 4 bit]
    uint8_t version_and_header_length = 0x45;  // 1 byte
    uint8_t service_type = 0x00;   // 1 byte
    uint16_t packet_length = 0;    // 2 bytes
    uint16_t packet_ID = htons(rand() % 65536);   // 2 bytes, 0~65535
    // [flags, 3 bits][offset, 13 bytes]
    uint16_t slice_info = htons(0x4000);    // 2 bytes
    uint8_t ttl = 64;    // 1 byte
    uint8_t type_of_protocol = 0x06;   // 1 byte
    uint16_t ip_check_sum = 0;      // 2 bytes
    uint32_t source_addr = htonl(0x8BE0FD5C);   // 4 bytes
    uint32_t dest_addr = htonl(0x0A16D89D);     // 4 bytes
    // SJ fix IP header to be 20 bytes, so ip_info is not needed.
    //uint8_t ip_info[40];    // 0-40 bytes, must be divisible by 4
};

// TCP header, 20 bytes to 60 bytes.
const int kTcpMaxLength = 60;
struct HeaderTCP {
    uint16_t source_port = 1 + rand() % 65535;   // 2 bytes, 1~65535
    uint16_t dest_port = htons(80);     // 2 bytes
    uint32_t seq = rand() % 2147483648;   // 4 bytes, 0~2^31-1
    uint32_t ack = rand() % 2147483648;   // 4 bytes, 0~2^31-1
    // [data offset, 4 bit][all zeros, 3 bit][flags, 9 bit]
    uint16_t data_offset_and_flags = htons(0x9010);     // 2 bytes
    uint16_t window_size = rand() % 65536;   // 2 bytes, 0~65535
    uint16_t tcp_check_sum = 0;     // 2 bytes
    uint16_t urgent_pointer = 0;    // 2 bytes
    uint8_t tcp_info[40] {};    // 0-40 bytes, must be divisible by 4
};

// TCP pseudo header, 12 bytes(fixed).
const int kTcpPseudoLength = 12;
struct PseudoHeaderTCP {
    uint32_t source_addr = htonl(0x8BE0FD5C);   // 4 bytes
    uint32_t dest_addr = htonl(0x0A16D89D);     // 4 bytes
    uint8_t zeros = 0;          // 1 byte
    uint8_t protocol = 0x06;       // 1 byte
    uint16_t tcp_length = 0;    // 2 bytes
};