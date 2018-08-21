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
#include <signal.h> // signal child process
#include <time.h> // sleep function


using namespace std;

/**
 * start via ssh netperf-server on server for bandwidth measurement
 */
int netPerf_startOnServer()
{
    //TODO: setup correct ssh-befehl
    /*
     * the username & pw might be problematic, because of no public key on server (probably).
     * Might be becessary to start netperf-server by hand
     */
    //sth like: int err = system("ssh {user}@{host}");
    return 0;
}

/**
 * free used udp pointer (socket will NOT be closed before freeing).
 * @param sock the udp-socket
 * @param dest the destination address (prob.ly some server)
 */

void udp_freePointer(int* sock, struct sockaddr_in * dest)
{
    free(sock);
    free(dest);
}

/**
 * Setup the udp socket and save setup data into the pointers sock and dest.
 * @param udp_sock stores the udp-socket
 * @param udp_dest stores the address of the destination
 * @param destIp the destination ip as string
 * @param port the destination port as integer
 * @return if some error occure a -1 is returnd
 */
int udp_setupConnection(int * udp_sock, struct sockaddr_in * udp_dest, string destIp, int port, int udp_socket_timeout)
{
    *udp_sock = socket(AF_INET, SOCK_DGRAM, 0); //IPv4, UDP, standard Protocoll Field in IP Header
    if (*udp_sock < 0) {
        printf("Error while opening the udp socket\n");
        udp_freePointer(udp_sock, udp_dest);
        return -1;
    }

    //set sockopt
    struct timeval timeout;
    timeout.tv_sec = udp_socket_timeout;
    timeout.tv_usec = 0;
    if (0 > setsockopt(*udp_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout)))
    {
        udp_freePointer(udp_sock, udp_dest);
        return -1;
    }
    if (0 > setsockopt(*udp_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout)))
    {
        udp_freePointer(udp_sock, udp_dest);
        return -1;
    }

    //Destination Address
    (*udp_dest).sin_family = AF_INET;
    (*udp_dest).sin_port = htons(port);
    (*udp_dest).sin_addr.s_addr = inet_addr(destIp.c_str());
    if((*udp_dest).sin_addr.s_addr < 0)
    {
        perror("Invalid address/ Address not supported\n");
        udp_freePointer(udp_sock,udp_dest);
        return -1;
    }

    return 0;
}



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
 * Listen for data at a specific udp_socket until the socket-timeout cancels connection and free resources.
 * @param udp_sock the udp-socket
 * @return 0, if udp_sock is closed by timeout, else -1
 */

int udp_listen4data(int* udp_sock)
{
    struct sockaddr_in *clientAddr;
    socklen_t slen = (socklen_t) sizeof(clientAddr);
    char buf[1440]; // TODO set packetsize

    while(1)
    {
        if (0 > recvfrom(*udp_sock, buf, (size_t) (sizeof(buf)), 0, (sockaddr *) clientAddr, (__socklen_t *) &slen))
        {
            if (11 == errno) // timeout error
            {
                return 0;
            } else {
                printf("Errno %i\n", errno);
                printf("Some error at receiving dataa\n");
                fflush(stdout);
                return -1;
            }

        }
        //printf("Data: %s\n" , buf);
        //fflush(stdout);
    }
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
 * Annotation to parameters:
 * The standard netperf performance call lasts 10 seconds, so the timeout parameter should be higher than 10, so the server does not
 * close the udp_port before it is created for the generation of the .pcap file.
 */

int main(int argc, char **argv) {
    if(argc < 8){
        cout << "Usage: ./Client [TECHNOLOGIE_ORT_REGION] [direction] [Server-IP] [TCP-Port] [UDP-Port] [UDP_timeout] [local_device]" << endl;
        // Example ./Client lte_osna_stadt b 127.0.0.1 8080 8081 20 lo
        return 0;
    }

    string measurementid = argv[1]; /* is shared with server via tcp */
    string direction = argv[2]; // b for bidirectional measurement, u for unidirectional measurement
    string destIp = argv[3];
    int tcp__port = atoi(argv[4]);
    int udp__port = atoi(argv[5]);
    int udp_socket_timeout = atoi(argv[6]); /* is shared with server via udp */
    char* local_dev =argv[7]; /* only on client computer valid */

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
    printf("Echo packets: %s\n", direction.c_str() );
    printf("Destination ip: %s\n", destIp.c_str() );
    printf("TCP port: %i\n", tcp__port );
    printf("UDP port: %i\n", udp__port );
    printf("UDP socket timeout: %i\n", udp_socket_timeout);
    printf("Local device: %s\n", local_dev);
    printf("*********************************************************************\n");

    double dummyload = 8;           //TODO: give this variable more sense
    int simulateLoadSeconds = 10;   //TODO: give this variable more sense

    // start netperf-server on Server for BW measurement
    if (0 > netPerf_startOnServer())
    {
        perror("measure bandwidth: failed \n");
        return -1;
    };

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
    if (0 > tcp_sendParametersForMeasurement(tcp_sock, measurementid, direction, udp_socket_timeout))
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
    string netperf_params = "netperf -H 127.0.0.1 -t UDP_STREAM -- -R 1 >> netperf_bandwidth_logger.txt"; //TODO: Does this parameter make sense?
    if (0 > measureThroughputViaUDP_Stream(bw_file.c_str(), netperf_params))
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

        sleep(5); // TODO: hopefully child starts living before then (is almost always the case)
        printf("child_pcap pid: %i\n", child_pcap);

        iperf_generateLoadClient(destIp,udp__port,1,'m');

        /*
         * clean up the processes, threads, mallocs,...
         * udp_sock is closed by timeout in send and recv which results closing the udp_socket
         */
        close(*tcp_sock);
        tcp_freePointer(tcp_sock,tcp_dest);
        kill(child_pcap, SIGINT); /* ends PcapWriter Process and its threads*/



        //TODO: extract signal strength data from signal strength document
        //TODO: and terminate module (e.g. "system(sudo rmmod example_module);")

    }
    return 0;
}
