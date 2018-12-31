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
#define kDat "network.dat"
#define kReadDat "network.dat"
//#define kReadDat "../network1.dat"

#define GRACEFUL_RETURN(s, x) {\
    perror((s));\
    return((x)); }

void generate_init_vector(char *buf, const int len);

char* process_init(int segment_id);

int allocate_shared_memory(int shared_segment_size);

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

uint16_t checksum_TCP_sender(const char *buf, const int buflen, const PseudoHeaderTCP pseudo);
