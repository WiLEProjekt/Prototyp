#include "../MeasureBandwidth/netperfBandwidth.h"
#include "../generateLoad/generateLoad.h"
#include "../consts_and_utils/consts_and_utils.h"
#include "../PcapWriter/PcapWriter.h"

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
#include <mutex>
#include <sys/shm.h>
#include <boost/program_options.hpp>


using namespace std;
using namespace boost::program_options;

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
    options_description desc{"Options"};
    string measurementid; /* is shared with server via tcp */
    string destIp;
    int tcp__port;
    int udp__port;
    string device; /* only on client computer valid */
    desc.add_options()
            ("help,h", "Help screen")
            ("identifier,i", value<string>(&measurementid), "Technology_Place_Region")
            ("destip,d", value<string>(&destIp), "IP-Address of the Server")
            ("porttcp,t", value<int>(&tcp__port), "TCP-Port of the Server")
            ("portudp,u", value<int>(&udp__port), "UDP-Port of the Server")
            ("device,e", value<string>(&device), "Name of the networkdevice of your computer");
    try{
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if(vm.count("help")){
            cout << desc << endl;
            return 0;
        }else if(!(vm.count("identifier") && vm.count("destip") && vm.count("porttcp") && vm.count("portudp") && vm.count("device"))){
            cout << "Missing parameters. All parameters need to be provided." << endl;
            return -1;
        }
    }catch(const error &ex){
        cerr << ex.what() << endl;
    }
    char* local_dev = new char[device.length() + 1]; /* only on client computer valid */
    strcpy(local_dev, device.c_str());

    // TODO: read parameters via -p annotation

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
    if (0 > measureThroughputViaUDP_Stream(bw_file.c_str(), netperf_params, &upload_bw, &download_bw))
    {
        perror("measureThroughputViaUDP_Stream - error: failure at measuring bandwidth\n");
        return -1;
    }
    printf("Measuring throughput successfully (%s)\n", bw_file.c_str());

    /* PcapWriter has to be started before parent and has to be forecefully terminated via SIGINT.
     * So a shared memory contains a mutex which assures that parent will start after child_pcap started.
     */

    key_t key = ftok("shm_pcap_parent_racing_conditions", 84);
    int shmid = shmget(key, sizeof(mutex), IPC_CREAT);
    mutex *mt_ptr = (mutex *) shmat(shmid,0,0);
    mutex mtx;
    memcpy(mt_ptr, &mtx,sizeof(mtx));
    (*mt_ptr).lock(); // lock mutex for child and parent

    pid_t child_pcap = fork();
    if (child_pcap == 0)
    {
        /* child does not have to listen what the tcp_socket has to say ;) */
        tcp_freePointer(tcp_sock, tcp_dest);

        string pcapfile = dirpath;
        pcapfile = pcapfile + "/" + measurementid + ".pcap";
        PcapWriter *writer = new PcapWriter();
        (*mt_ptr).unlock(); // child has started already
        writer->start(local_dev, stringToChar(pcapfile)); /* some threads are started here, thos will be closed by SIGINT */
    } else {

        /* wait until child unlocks the mutex in the shm */
        while (!(*mt_ptr).try_lock())
        {
            sleep(1);
        }
        shmdt(mt_ptr); // detach pointer from shm
        shmctl(shmid,IPC_RMID, NULL); // release shm

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
