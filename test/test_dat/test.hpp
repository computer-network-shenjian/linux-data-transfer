#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdint.h>

#define kRawDat "sender.dat"

bool write_dat(const unsigned char *buf, const unsigned int buflen, const char *dat_name);
