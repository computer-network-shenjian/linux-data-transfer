#include "types.hpp"

class Config {
    public:
    Config():datalen(1024)
    {
    // Here comes a config reading function
    }
    // Application layer
    unsigned int datalen;

    // Transport layer
    HeaderTCP tcp_header;

    // IP layer
    HeaderIP ip_header;

    // MAC layer
    HeaderMAC mac_header;
}
