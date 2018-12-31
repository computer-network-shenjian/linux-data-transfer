#include "test.hpp"
using namespace std;

int main() {
    unsigned int buflen = 20;
    unsigned char *buf = new unsigned char[20];
    buf[0] = 0x12;
    buf[1] = 0x3F;
    buf[2] = 0xEA;
    write_dat(buf, buflen, kRawDat);
    delete[] buf;
    return 0;
}

bool write_dat(const unsigned char *buf, const unsigned int buflen, const char *dat_name) {
    ofstream ofs(dat_name, ofstream::trunc);
    if (!ofs.is_open()) {
        cerr << "write_dat(): open file error" << endl;
        return false;
    }
    for (unsigned int i = 0; i < buflen; i++) {
        ofs << setfill('0') << setw(2);
        ofs << hex << (unsigned short int)buf[i] << ' ';
        if (i % 16 == 15) {
            ofs << endl;
        }
    }
    ofs.close();
    return true;
}