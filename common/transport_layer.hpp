#include "utils.hpp"

const std::string fn_sender = "fn_sender.dat";
const std::string fn_receiver = "fn_receiver.dat";

int sender_transport_layer(int lower_layer_pid, int segment_id, CopyMode copy_mode);

int receiver_transport_layer(int higher_layer_pid, int segment_id);
     
bool flag_ready_to_exit = false;
void signal_handler(int sig);
