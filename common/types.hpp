// Note: Enumerators should be named like constants. See Google C++ Style Guide for detail
enum class CopyMode {
    Copy,
    Shared
};

// Frame layout:
//
// +------------+-------------------+-----------+------------+------------------------------------------------------+
// | Field name |       Layer       |   Size    |   Range    |                       Content                        |
// +------------+-------------------+-----------+------------+------------------------------------------------------+
// | MAC header | datalink layer    | 14 bytes   | [0, 14)    | Source/destination MAC address                       |
// | IP header  | network layer     | 20 bytes   | [14, 34)   | source/destination IP, TTL                           |
// | TCP header | transport layer   | 20 bytes   | [34, 54)   | Window size, seq_nr, ack seq_nr, port number, etc... |
// | Payload    | application layer | 1460 bytes | [54, 1514) | Random data                                          |
// +------------+-------------------+-----------+------------+------------------------------------------------------+
//
// the position of the first byte in a packet of each field
// note: the range is left-closed and right-open
enum class PacketFieldPos : unsigned int {
    MacHeader = 0,
    IpHeader = 14,
    TcpHeader = 34,
    Payload = 54,
    PacketEnd = 1514
};

const int payload_length = PacketFieldPos::PacketEnd - PacketFieldPos::Payload;
const unsigned int kPayloadLength = PacketFieldPos::PacketEnd - PacketFieldPos::Payload;
const unsigned int kTcpHeaderLength = PacketFieldPos::Payload - PacketFieldPos::TcpHeader;
const unsigned int kIpHeaderLength = PacketFieldPos::TcpHeader - PacketFieldPos::IpHeader; 
const unsigned int kMacHeaderLength = PacketFieldPos::IpHeader - PacketFieldPos::MacHeader;
// for CopyMode::copy
const unsigned int kTcpHeaderCopyLength = PacketFieldPos::PacketEnd - PacketFieldPos::TcpHeader;
const unsigned int kIpHeaderCopyLength = PacketFieldPos::PacketEnd - PacketFieldPos::IpHeader;
const unsigned int kMacHeaderCopyLength = PacketFieldPos::PacketEnd - PacketFieldPos::MacHeader;

// error codes
enum class ErrorCode : int {
    OK = 0,
    OpenFile = -1,
    SharedMemoryAllocation = -2,
    SharedMemoryAttachment = -3,
    LogInit = -4,
    WrongCopyMode = -5,
    WrongHeaderType = -6
};

enum class Protocol : uint8_t {
    TCP = 6
};

enum class HeaderType : int {
    TCP = 0,
    IP = 1
};

// other constants
const string fn_physical_layer = "physical_layer.dat";
//const uint16_t kTcpSourcePort = 12345;
//const uint16_t kTcpDestPort = 6789;
//const uint32_t kSourceAddr = 0;
//const uint32_t kDestAddr = 0;
