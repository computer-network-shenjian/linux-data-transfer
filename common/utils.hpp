#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "types.hpp"

#define SIGGOOD SIGUSR1
#define SIGBAD SIGUSR2

#define kRawSendDat "sender.dat"
#define kRawRecvDat "receiver.dat"
#define kDat = "network.dat"
#define kReadDat "../network1.dat"

// A macro that gracefully return by first calling perror to
// display a decent error message with error string s,
// then exit with error code x.
#define GRACEFUL_RETURN(s, x) {\
    perror((s));\
    return((x)); }

// This function generates an array of random 
// uint8_t documents the intent: will be storing small numbers, 
// rather than characters
//
// Example usage:
// int main() {
//     uint8_t data[16];
//     generate_init_vector(data);
// }
template<std::size_t N>
void generate_init_vector(uint8_t (&IV_buff)[N]);

// Initialize a process right after entring a function of a layer
// The initialization is a x stage process:
// 1. Register death of child when parent dies
// 2. Attach shared memory and return its address
// 3. register proper signal handlers
char* process_init(int segment_id);

// Allocate and zero-initialize a shared memory segment, returning sengment_id
int allocate_shared_memory(int shared_segment_size);

// Deallocate the shared memory segment specified by segment_id
int deallocate_shared_memory(int segment_id);

bool write_dat(const char *buf, const unsigned int buflen, const char *dat_name);

int read_dat(char *buf, const char *dat_name);

void print_header_MAC(const HeaderMAC header);

void print_header_IP(const HeaderIP header);

uint16_t checksum_IP(const HeaderIP header);

void print_header_TCP(const HeaderTCP header, const uint16_t check_sum);

uint16_t checksum_TCP(const char *buf);

int get_dat_length(const char *dat_name);

int get_tcp_data_length(const HeaderTCP header);

void print_tcp_data(const char *buf, const int len);