#include "utils.hpp"

// This function generates random data of of length between 1-1460 bytes
// and write to shared memory at [kPayloadPos, kPacketEndPos).
//
// The shared memory to be attached is assumed to be big enough
//
// Implementation note: the shared memory can be attached by
//      char *shared_memory = (char*) shmat (segment_id, 0, 0);
// 
// This functions is the same for both copy and shared modes.
int sender_application_layer(int segment_id);
