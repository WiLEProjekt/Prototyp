#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <vector>

using namespace std;

unsigned int parseNumberFromBytes(unsigned char *bytes, int length) {
    unsigned int number = 0;
    for (int i = length - 1; i >= 0; i--) {
        number += bytes[length - i - 1] * pow(256, i);
    }

    return number;
}

string parseIP(unsigned char *bytes) {
    string ip;
    for (int i = 0; i < 4; i++) {
        unsigned char numberByte[] = {bytes[i]};
        unsigned int number = parseNumberFromBytes(numberByte, 1);
        ip += std::to_string(number);
        if (i < 4) {
            ip += ".";
        }
    }
    return ip;
}

vector<struct timeval> readPcapFile(const string &filename, string sourceIp, string destIp){
    vector<struct timeval> timestamps;
    struct stat buffer{};
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }

            unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
            unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};

            //check if packet is UDP (17)
            if(data[24] == 17) {
                if (sourceIp == parseIP(sourceIPBytes) && destIp == parseIP(destIPBytes)) {
                    timestamps.push_back(header->ts);
                }
            }
        }
        pcap_close(pcap);
    }
    return timestamps;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}