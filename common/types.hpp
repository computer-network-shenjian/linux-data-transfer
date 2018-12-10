const string fn_physical_layer = "physical_layer.dat";

// Note: Enumerators should be named like constants. See Google C++ Style Guide for detail
enum CopyMode {
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
enum FramePos {
    kMacHeaderPos = 0,
    kIpHeaderPos = 14,
    kTcpHeaderPos = 34,
    kPayloadPos = 54,
    kPacketEndPos = 1514,
};
