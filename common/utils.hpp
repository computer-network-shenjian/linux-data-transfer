#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <functional>

#include <iostream>
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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include "Log.h"


// This function generates an array of random 
// example usage
// int main() {
//     uint8_t data[16];
//     generate_init_vector(data);
// }
template<std::size_t N>
void generate_init_vector(uint8_t (&IV_buff)[N]);
