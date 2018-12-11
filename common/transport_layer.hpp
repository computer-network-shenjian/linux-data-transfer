#include "utils.hpp"

// overall variables
bool former_not_ready = true;

// functions
int sender_transport_layer(int lower_layer_pid, int segment_id, CopyMode copy_mode);

int receiver_transport_layer(int higher_layer_pid, int segment_id);
     
void signal_handler(int sig);

void generate_tcp_header(uint8_t *ptr_tcp_header);

void parse_tcp_header(uint8_t *ptr_tcp_header);

void tcp_header_printer(const HeaderTcp header);
