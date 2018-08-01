#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <cstdint>
#include <vector>

using namespace std;

uint64_t getTimens(){
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    uint64_t s = time.tv_sec;
    uint64_t ns = time.tv_nsec;
    uint64_t finaltime = s*1000000000+ns;
    return finaltime;
}

int main(int argc, char **argv) {
    //Open Socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return 0;
    }

    //Own Address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4722);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind own address to socket
    int bnd = bind(sock, (struct sockaddr *) &addr, sizeof (struct sockaddr_in));
    if (bnd < 0) {
        return 0;
    }

    //set timeout on receive function
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    int socktimeout = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout));
    if (socktimeout < 0) {
        return 0;
    }
    int size = 0;
    //Bandwidth measurement using packet pair method
    vector<vector<uint64_t> > receivetimes; //[sequencenumber], [arrivaltime in ns]
    char rcvmsg[1450];
    struct sockaddr_in from;
    unsigned int flen = sizeof (struct sockaddr_in);
    int oldsequencenumber = -1;
    while(true){
        vector<uint64_t> temp;
        int rcv = recvfrom(sock, rcvmsg, sizeof(rcvmsg), 0, (struct sockaddr*) &from, &flen);
        if (rcv < 0) { //exit upload-measurement
            break;
        }else{
            uint64_t time = getTimens();
            string payload = string(rcvmsg);
            int sequencenumber = stoi(payload);
            //cout << sequencenumber << endl;
            size = rcv;
            if(sequencenumber>oldsequencenumber){ //filter duplicates and reordered packets
                oldsequencenumber = sequencenumber;
                temp.push_back(sequencenumber);
                temp.push_back(time);
                receivetimes.push_back(temp);
            }

        }
    }
    //Calculate bandwidth in kbit/sec
    vector<int> bandwidths;
    int loop = 0;
    if(receivetimes.size() >1){
        while(loop<=receivetimes.size()-2){
            if((receivetimes[loop][0]%2) == 0 && (receivetimes[loop+1][0]%2) == 1 && (receivetimes[loop+1][0] == receivetimes[loop][0]+1)){//Packet Pair found
                long double sizekb = (long double) size/125; //Convert byte into kilobit
                long double time1 = (long double)receivetimes[loop+1][1]/1000000000;
                long double time0 = (long double)receivetimes[loop][1]/1000000000;
                long double bandwidth = sizekb/(time1-time0);
                int roundedbandwidth = (int) (bandwidth+0.5);
                bandwidths.push_back(roundedbandwidth);
                loop = loop + 2;
            }else{
                loop++;
            }
        }
    }else{
        cout << "No packet pair found" << endl;
        return 0;
    }
    if(bandwidths.size()<1){
        cout << "No packet apir found" << endl;
        return 0;
    }

    for(int i = 0; i<bandwidths.size();i++){
        cout << bandwidths[i] << "kbit/s" << endl;
    }
    return 0;
}


