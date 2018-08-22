#include "MeasureBandwidth/netperfBandwidth.cpp"
#include "generateLoad/generateLoad.h"
#include "consts_and_utils/consts_and_utils.h"
#include "PcapWriter/PcapWriter.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <algorithm>
#include <signal.h> // signal child process
#include <time.h> // sleep function


using namespace std;







/**
 * Free used tcp pointer (socket will NOT be closed before).
 * @param sock the udp-socket
 * @param dest the destination address (prob.ly some server)
 */
void tcp_freePointer(int* tcp_sock, struct sockaddr_in *tcp_dest)
{
    free(tcp_sock);
    free(tcp_dest);
}

/**
 * Initializes a tcp connection and stores relevant data in tcp_sock and tcp_destaddr
 * @param tcp_sock the tcp socket
 * @param tcp_destaddr the destination address
 * @param destIp destionation by dotted ip
 * @param port the port
 * @return -1 if tcp connection could not be created, else 0
 */
int tcp_initializeConnectionClient(int* tcp_sock, struct sockaddr_in *tcp_destaddr, string destIp, int port)
{
    *tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*tcp_sock < 0)
    {
        perror("Error while opening the Socket\n");
        tcp_freePointer(tcp_sock,tcp_destaddr);
        return -1;
    }

    // destination address
    memset(tcp_destaddr, '0', sizeof(tcp_destaddr));
    (*tcp_destaddr).sin_family = AF_INET;
    (*tcp_destaddr).sin_port = htons(port);
    (*tcp_destaddr).sin_addr.s_addr = inet_addr(destIp.c_str());
    if((*tcp_destaddr).sin_addr.s_addr < 0)
    {
        perror("Invalid address/ Address not supported\n");
        tcp_freePointer(tcp_sock,tcp_destaddr);
        return -1;
    }

    // connect to tcp_destaddr
    socklen_t t = sizeof(*tcp_destaddr);
    if (0 > connect(*tcp_sock,(struct sockaddr *) tcp_destaddr, (socklen_t) sizeof(*tcp_destaddr)))
    {
        perror("Connection Failed\n");
        tcp_freePointer(tcp_sock,tcp_destaddr);
        return -1;
    }
    return 0;
}



/**
 * This function transmits relevant parameter data from client to server via a tcp connection. The data is delimited by '|'.
 * Example: lte_osna_stadt_16082018_011623|b|10
 * @param tcp_sock tcp connection socket
 * @param measurementId unique measurement id
 * @param d a uni or bidirectional parameter
 * @param udp_timeoutServer the server timeout
 * @return -1 if parameter transmission failed
 */
int tcp_sendParametersForMeasurement(int* tcp_sock, string measurementId, string d, int udp_timeoutServer)
{
    bool trans_sucessfull = false;
    string params = "|" + measurementId + "|" + d + "|" + to_string(udp_timeoutServer) +"|";
    while (not trans_sucessfull)
    {
        char sendmsg[PACKETSIZE];
        memset(sendmsg,0, sizeof(sendmsg));
        memcpy(sendmsg,params.c_str(),strlen(params.c_str()));
        if (0 > send(*tcp_sock , sendmsg , strlen(sendmsg) , 0 ))
        {
            return -1;
        }
        char recvmsg[PACKETSIZE];
        memset(recvmsg,0, sizeof(sendmsg));
        read(*tcp_sock , recvmsg , strlen(recvmsg));
        if (strcmp(recvmsg, "ack"))
        {
            return 0;
        }
    }
}

/**
 * This function transmits relevant parameter data from client to server via a tcp connection. The data is delimited by '|'.
 * Example: lte_osna_stadt_16082018_011623|b|10
 * @param tcp_sock tcp connection socket
 * @param measurementId unique measurement id
 * @return -1 if parameter transmission failed
 */
int tcp_sendParametersForMeasurement(int* tcp_sock, string measurementId)
{
    bool trans_sucessfull = false;
    string params = "|" + measurementId + "|";
    while (not trans_sucessfull)
    {
        char sendmsg[PACKETSIZE];
        memset(sendmsg,0, sizeof(sendmsg));
        memcpy(sendmsg,params.c_str(),strlen(params.c_str()));
        if (0 > send(*tcp_sock , sendmsg , strlen(sendmsg) , 0 ))
        {
            return -1;
        }
        char recvmsg[PACKETSIZE];
        memset(recvmsg,0, sizeof(sendmsg));
        read(*tcp_sock , recvmsg , strlen(recvmsg));
        if (strcmp(recvmsg, "ack"))
        {
            return 0;
        }
    }
}


/**
 * Signal server to end connection and clean up processes, threads and malloc gracefully.
 * @param tcp_sock the tcp connection
 * @return -1 if some error occures, else 0
 */
int tcp_signalServerToCleanUp(int* tcp_sock)
{
    string params = "cleanUp";
    char sendmsg[PACKETSIZE];
    memset(sendmsg,0, sizeof(sendmsg));
    memcpy(sendmsg,params.c_str(),strlen(params.c_str()));
    if (0 > send(*tcp_sock , sendmsg , strlen(sendmsg) , 0 ))
    {
        printf("errno %i\n", errno);
        return -1;
    }
    return 0;
}


int main(int argc, char **argv) {
    if(argc < 5){
        cout << "Usage: ./Client [TECHNOLOGIE_ORT_REGION] [direction] [Server-IP] [TCP-Port] [UDP-Port] [UDP_timeout] [local_device]" << endl;
        // Example ./Client lte_osna_stadt b 127.0.0.1 8080 8081 20 lo
        return 0;
    }
    // TODO: read parameters via -p annotation

    string measurementid = argv[1]; /* is shared with server via tcp */
    string destIp = argv[2];
    int tcp__port = atoi(argv[3]);
    int udp__port = atoi(argv[4]);
    char* local_dev =argv[5]; /* only on client computer valid */

    /* append current system date and time to generate unique measurement id */
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,sizeof(buffer),"%d%m%Y_%I%M%S",timeinfo);
    measurementid = measurementid + "_" + buffer;

    printf("********************************************************************\n");
    printf("*****************************Parameters*****************************\n");
    printf("Measurement id: %s\n", measurementid.c_str());
    printf("Destination ip: %s\n", destIp.c_str() );
    printf("TCP port: %i\n", tcp__port );
    printf("UDP port: %i\n", udp__port );
    printf("Local device: %s\n", local_dev);
    printf("*********************************************************************\n");

    /* create tcp connection to send some measurement parameters to server*/
    int *tcp_sock =(int*) malloc(sizeof(int) * 1);
    struct sockaddr_in *tcp_dest = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    if (0 < tcp_initializeConnectionClient(tcp_sock, tcp_dest, destIp, tcp__port))
    {
        printf("tcp_initializeConnectionClient - error: %s\n", strerror(errno));
        tcp_freePointer(tcp_sock,tcp_dest);
        return -1;
    }
    printf("TCP Connection established successfully\n");
    if (0 > tcp_sendParametersForMeasurement(tcp_sock, measurementid))
    {
        printf("tcp_sendParametersForMeasurement - error: %s\n", strerror(errno));
        tcp_freePointer(tcp_sock,tcp_dest);
        return -1;
    }
    printf("Parameter exchange between client and server successfull\n");

    /* make writeable directory at CLIENT_LOCATION */
    string dirpath = CLIENT_LOCATION;
    dirpath = dirpath + "/" + measurementid;
    if (0 > makeDirectoryForMeasurement(dirpath))
    {
        printf("makeDirectoryForMeasurement - error: directory creation failed\n");
        return -1;
    }
    printf("Creation of measurement directory successfull (%s)\n", dirpath.c_str());

    /* measure signal strength */
    //TODO: start logging signal strength stuff with module and write content to file


    /* measure bandwidth with saturated load*/
    string bw_file = dirpath;
    bw_file = bw_file + "/" + measurementid + "_throughput.txt";
    string netperf_params = "netperf -H ";
    netperf_params = netperf_params + destIp + " -t UDP_STREAM -- -R 1 >> netperf_bandwidth_logger.txt";
    double download_bw = 0.0, upload_bw = 0.0;
    if (0 > measureThroughputViaUDP_Stream(bw_file.c_str(), netperf_params, &download_bw,&upload_bw ))
    {
        perror("measureThroughputViaUDP_Stream - error: failure at measuring bandwidth\n");
        return -1;
    }
    printf("Measuring throughput successfully (%s)\n", bw_file.c_str());

    pid_t child_pcap = fork();
    if (child_pcap == 0)
    {
        /* child does not have to listen what the tcp_socket has to say ;) */
        tcp_freePointer(tcp_sock, tcp_dest);

        string pcapfile = dirpath;
        pcapfile = pcapfile + "/" + measurementid + ".pcap";
        PcapWriter *writer = new PcapWriter();
        writer->start(local_dev, stringToChar(pcapfile)); /* some threads are started here, thos will be closed by SIGINT */
    } else {

        /* load generation via iperf */
        printf("Start generation of load via iperf\n");
        int d = (fmin(download_bw,upload_bw) / 2);
        if ( 0 > iperf_generateLoadClient(destIp,udp__port,d,'m')) // m stands for mega bit per sec, this is a blocking call
        {
            perror("iperf_generateLoadClient - error: failure at generating load\n");
            return -1;
        }
        printf("Generation of load via iperf ended successfully\n");

        /* signal server to close connection via tcp */
        if (0 > tcp_signalServerToCleanUp(tcp_sock)) // this is a blocking call
        {
            perror("tcp_signalServerToCleanUp - error: failure at cleaning up\n");
            return -1;
        }
        printf("Signal server to close connection ended successfully\n");

        /*
         * clean up the processes, threads, mallocs,...
         * udp_sock is closed by timeout in send and recv which results closing the udp_socket
         */
        close(*tcp_sock);
        tcp_freePointer(tcp_sock,tcp_dest);
        kill(child_pcap, SIGINT); /* ends PcapWriter Process and its threads*/
        printf("Measurment completed successfully\n");
    }
    return 0;
}
