#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>

#define kConfFile "../../network.conf"

// MAC header, 14 bytes(fixed).
struct HeaderMAC {
    uint8_t mac_dest_addr[6] {};   // 6 bytes
    uint8_t mac_src_addr[6] {};    // 6 bytes
    uint16_t protocol_type = 0x0800;     // 2 bytes
};

// IP header, 20 bytes to 60 bytes, SJ fix it to be 20 bytes.
struct HeaderIP {
    // [version, 4 bit][header length, 4 bit]
    uint8_t version_and_header_length = 0x45;  // 1 byte
    uint8_t service_type = 0x00;   // 1 byte
    uint16_t packet_length = 0;    // 2 bytes
    uint16_t packet_ID = rand % 65536;   // 2 bytes, 0~65535
    // [flags, 3 bits][offset, 13 bytes]
    uint16_t slice_info = 0;    // 2 bytes
    uint8_t ttl = 64;    // 1 byte
    uint8_t type_of_protocol = 0x06;   // 1 byte
    uint16_t ip_check_sum = 0;      // 2 bytes
    uint32_t source_addr = 0;   // 4 bytes
    uint32_t dest_addr = 0;     // 4 bytes
    // SJ fix IP header to be 20 bytes, so ip_info is not needed.
    //uint8_t ip_info[40];    // 0-40 bytes, must be divisible by 4
};

// TCP header, 20 bytes to 60 bytes.
struct HeaderTCP {
    uint16_t source_port = 1 + rand() % 65535;   // 2 bytes, 1~65535
    uint16_t dest_port = 80;     // 2 bytes
    uint32_t seq = rand() % 2147483648;   // 4 bytes, 0~2^31-1
    uint32_t ack = rand() % 2147483648;   // 4 bytes, 0~2^31-1
    // [data offset, 4 bit][all zeros, 3 bit][flags, 9 bit]
    uint16_t data_offset_and_flags = 0x5010;     // 2 bytes
    uint16_t window_size = rand % 65536;   // 2 bytes, 0~65535
    uint16_t tcp_check_sum = 0;     // 2 bytes
    uint16_t urgent_pointer = 0;    // 2 bytes
    uint8_t tcp_info[40] {};    // 0-40 bytes, must be divisible by 4
};
