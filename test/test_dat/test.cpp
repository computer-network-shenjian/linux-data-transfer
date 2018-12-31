#include "test.hpp"
using namespace std;

int main() {
    unsigned int buflen = 20;
    unsigned char *buf = new unsigned char[20];
    buf[0] = 'h';
    buf[1] = 'e';
    buf[2] = 'l';
    buf[3] = 'l';
    buf[4] = 'o';
    write_dat(buf, buflen, kRawDat);
    delete[] buf;

    buf = new unsigned char[20];
    cout << "dat length: " << read_dat(buf, kRawDat) << endl;
    cout << buf << endl;
    delete[] buf;

    buf = new unsigned char[1024];
    buflen = read_dat(buf, kReadDat);
    cout << "dat length: " << buflen << endl;

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

int read_dat(unsigned char *buf, const char *dat_name) {
    ifstream ifs(dat_name);
    if (!ifs.is_open()) {
        cerr << "read_dat(): open file error" << endl;
        return -1;
    }
    
    // b[0] is the first char of hex, b[1] is the second char of hex.
    char b[3] = {0};
    int cnt = 0;
    while (ifs.good()) {
        b[0] = ifs.get();
        if (b[0] == -1) {
            break;
        }
        if (b[0] == '\n') {
            b[0] = ifs.get();
        }
        b[1] = ifs.get();
        ifs.get();
        stringstream ss;
        ss << b;
        unsigned int a;
        ss >> hex >> a;
        buf[cnt] = a;
        cnt++;
    }

    ifs.close();
    return cnt;
}
