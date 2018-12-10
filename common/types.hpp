const string fn_physical_layer = "physical_layer.dat";

// Note: Enumerators should be named like constants. See Google C++ Style Guide for detail
enum class CopyMode {
    kCopy,
    kShared,
};

// Frame layout:
//
// +------------+-------------------+-----------+------------+------------------------------------------------------+
// | Field name |       Layer       |   Size    |   Range    |                       Content                        |
// +------------+-------------------+-----------+------------+------------------------------------------------------+
// | MAC header | datalink layer    | 14 byts   | [0, 14)    | Source/destination MAC address                       |
// | IP header  | network layer     | 20 byts   | [14, 34)   | source/destination IP, TTL                           |
// | TCP header | transport layer   | 20 byts   | [34, 54)   | Window size, seq_nr, ack seq_nr, port number, etc... |
// | Payload    | application layer | 1460 btes | [54, 1514) | Random data                                          |
// +------------+-------------------+-----------+------------+------------------------------------------------------+
//
// the position of the first byte in a packet of each field
// note: the range is left-closed and right-open
enum class PacketFieldPos : unsigned int {
    MacHeader = 0,
    IpHeader = 14,
    TcpHeader = 34,
    Payload = 54,
    PacketEnd = 1514,
};

// error codes
enum class ErrorCode : int {
    OK = 0;
    OpenFile = -1,
    SharedMemoryAllocation = -2,
    SharedMemoryAttachment = -3,
}

// Simply use SIGUSR1 to indicate shared memory ready
// and SIGUSR2 to indicate it's ok to exit the process
//
// enum class CustomSignal : int {
// 
// }
