#include "utils.hpp"

const std::string fn_sender = "fn_sender.dat";
const std::string fn_receiver = "fn_receiver.dat";

// This function generates random data of of length between 1-1460 bytes
// and write to both file fn_sender and shared memory at position [PacketFieldPos::Payload, PacketFieldPos::PacketEnd).
//
// The shared memory to be attached and written to is assumed to be big enough
//
// The implementation of this layer is the same for both copy and shared modes.
int sender_application_layer(int lower_layer_pid, int segment_id);

// The receiver side application layer
// Unpack payload from higher layer from shared memory and write to file fn_receiver
// The payload is located at position 
int receiver_application_layer(int higher_layer_pid, int segment_id);

// The signal handler of the application layer handles two signals:
//  1. SIGUSR1
//      SIGUSR1 is ignored because there is no upper layers so no need to know the shared
//      memory is ready.
//  2. SIGUSR2
//      Set the flag_ready_to_exit global variable to true, so the 
//      main function can safely return from the busy-wait loop
//      
bool flag_ready_to_exit = false;
void signal_handler(int sig);
