//
// Created by dominic on 02.08.18.
//

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <cstdint>
#include <vector>
#include <algorithm>

using namespace std;

uint64_t getTimens(){
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    uint64_t s = time.tv_sec;
    uint64_t ns = time.tv_nsec;
    uint64_t finaltime = s*1000000000+ns;
    return finaltime;
}

int calculateAvgBandwidth(vector<int> origbandwidths){
    int epsilon = 5000; //+-5Mbit Schranken
    vector<int> bandwidthswithoutduplicates = origbandwidths;
    sort(bandwidthswithoutduplicates.begin(), bandwidthswithoutduplicates.end());
    vector<int>::iterator it;
    it = unique (bandwidthswithoutduplicates.begin(), bandwidthswithoutduplicates.end()); //erase duplicates
    bandwidthswithoutduplicates.resize( std::distance(bandwidthswithoutduplicates.begin(),it) );

    vector<int> consensus;
    for(int i = 0; i<bandwidthswithoutduplicates.size(); i++){
        int adder = 0; //temporal variable to calculate consensusset
        for(int o = 0; o<origbandwidths.size(); o++){
            if((origbandwidths[o]<=(bandwidthswithoutduplicates[i]+epsilon)) && (origbandwidths[o] >= (bandwidthswithoutduplicates[i]-epsilon))){
                adder++;
            }
        }
        consensus.push_back(adder);
    }

    //find biggest consensusset
    int max = 0;
    int maxi = 0;
    for(int i = 0; i<consensus.size(); i++){
        if(consensus[i]>max){
            max = consensus[i];
            maxi = i;
        }
    }

    //calculate mean value of best consensusset
    int overallbandwidth = 0;
    for(int i = 0; i<origbandwidths.size(); i++){
        if((origbandwidths[i]<=(bandwidthswithoutduplicates[maxi]+epsilon)) && (origbandwidths[i] >= (bandwidthswithoutduplicates[maxi]-epsilon))){
            overallbandwidth = overallbandwidth+origbandwidths[i];
        }
    }
    int meanbandwidth = overallbandwidth/consensus[maxi];
    return meanbandwidth;
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


    //Bandwidth measurement using packet pair method
    int size = 0;
    vector<vector<uint64_t> > receivetimes; //[sequencenumber], [arrivaltime in ns]
    char rcvmsg[30000];
    struct sockaddr_in from;
    unsigned int flen = sizeof (struct sockaddr_in);
    int oldsequencenumber = -1;
    while(true){
        vector<uint64_t> temp;
        int rcv = recvfrom(sock, rcvmsg, sizeof(rcvmsg), 0, (struct sockaddr*) &from, &flen);
        if (rcv < 0) { //exit upload-measurement
            break;
        }else{
            string payload = string(rcvmsg);
            int sequencenumber = stoi(payload);
            //cout << sequencenumber << endl;
            size = rcv;
            if(sequencenumber>oldsequencenumber){ //filter duplicates and reordered packets
                oldsequencenumber = sequencenumber;
                temp.push_back(sequencenumber);
                uint64_t time = getTimens();
                temp.push_back(time);
                receivetimes.push_back(temp);
            }

        }
    }
    //Calculate bandwidth in mbit/sec
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

    int estimatedBandwidth = calculateAvgBandwidth(bandwidths);
    cout << estimatedBandwidth << " kbit/s" << endl;

    return 0;
}