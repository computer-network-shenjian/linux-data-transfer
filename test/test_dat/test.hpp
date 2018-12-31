#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdint.h>

#define kRawDat "sender.dat"
#define kReadDat "../../network1.dat"

bool write_dat(const unsigned char *buf, const unsigned int buflen, const char *dat_name);

int read_dat(unsigned char *buf, const char *dat_name);
