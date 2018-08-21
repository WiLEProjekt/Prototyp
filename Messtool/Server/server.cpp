#include "server.h"
#include "generateLoad/generateLoad.h"
#include "consts_and_utils/consts_and_utils.h"


#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <wait.h>

using namespace std;

#include <sstream>
#include <iterator>
#include <regex>

#include "generateLoad/generateLoad.h"




/**
 * Setup the udp socket and save setup data into the pointers sock and dest.
 * @param sock stores the udp-socket
 * @param port the destination port as integer
 * @param timeoutServer the timeout of the socket (in seconds)
 * @return if some error occure a -1 is returned
 */
int udp_setupConnection(int * sock, int port, int timeoutServer)
{
    //Open Socket
    *sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock < 0)
    {
        printf("udp_setupConnection - Errno: %i\n", errno);
        return -1;
    }
    //set sockopt
    struct timeval timeout;
    timeout.tv_sec = timeoutServer;
    timeout.tv_usec = 0;
    if (0 > setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout)))
    {
        if (11 == errno)
        {
            return 0;
        } else {
            printf("udp_setupConnection - Errno: %i\n", errno);
            return -1;
        }
    }
    //Own Address
    struct sockaddr_in own_addr;
    own_addr.sin_family = AF_INET;
    own_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    own_addr.sin_port = htons(port);
    //Bind own address to socket
    if (0 > bind(*sock, (struct sockaddr *) &own_addr, sizeof (struct sockaddr_in)))
    {
        printf("udp_setupConnection - Errno: %i\n", errno);
        return -1;
    }
    return 0;
}

/**
 * A function receives packets via udp and echo it back the sender.
 * @param udp_sock the udp_socket
 * @return
 */

int udp_recvAndEcho(int *udp_sock)
{
    struct sockaddr_in addr;
    socklen_t slen = (socklen_t) sizeof(addr);
    char buf[PACKETSIZE];

    //keep listening for data
    while(1)
    {
        if (0 > recvfrom(*udp_sock,buf,(size_t) (sizeof(buf)),0,(sockaddr *) &addr, (__socklen_t *) &slen))
        {
            if (11 == errno)
            {
                return 0;
            } else {
                printf("udp_recvAndEcho - Errno: %i\n", errno);
                return -1;
            }
        }
        if (sendto(*udp_sock, buf, PACKETSIZE, 0, (struct sockaddr*) &addr, slen) == -1)
        {
            if (11 == errno)
            {
                return 0;
            } else {
                printf("udp_recvAndEcho - Errno: %i\n", errno);
                return -1;
            }
        }
    }
}

/**
 * Creates a tco connection for a specific port and stores data in tcp_sock
 * @param tcp_sock the tcp socket
 * @param port the tcp port
 * @return -1 if an error occur, else 0
 */

int tcp_initializeConnectionServer(int* tcp_sock, int port)
{
    //Open Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    //set sockopt
/*  struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    if (0 > setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout)))
    {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
*/
    //address
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    //bind own address to socket
    if (0 > bind(sock, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)))
    {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    // listen for connections
    if (listen(sock, 3) < 0)
    {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    //accepts connection
    int addrlen = sizeof(addr);
    if ((*tcp_sock = accept(sock, (struct sockaddr *)&addr,
                             (socklen_t*) &addrlen) ) < 0)
    {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    return 0;
}

/**
 * Free udp socket without close without closing socket before.
 * @param udp_sock the udp socket
 */

void udp_closeConnection(int *udp_sock)
{
    free(udp_sock);
}

/**
 * Free the tcp socket without closing socket before.
 * @param tcp_sock the tcp socket
 */

void tcp_closeConnection(int *tcp_sock)
{
    free(tcp_sock);
}

/**
 * Close tcp socket, free tcp pointer and kill a process child_pid
 * @param child_pid the child pid
 * @param tcp_sock the tcp socket
 */

void kill_child_end_tcp(pid_t child_pid, int* tcp_sock)
{
    close(*tcp_sock);
    free(tcp_sock);
    kill(child_pid, SIGINT); /* ends PcapWriter Process and its threads*/
    printf("child killed\n");
}

/**
 * Receive some measurement parameters via tcp. PArameters are checked and then saved into check_params. Return
 * -1 if parameters are invalid or some other error occured.
 * @param tcp_sock the tcp socket
 * @param checked_params a vector for the parameter 'timeout', 'direction' and 'measurment id'
 * @return -1 if parameters are invalid or soem other error occured
 */

int tcp_recvMeasurementParameters(int* tcp_sock, vector<string>* checked_params)
{
    /*
     * Expected Parameter: |[measurementId]|[direction]|[timeout]|
     * Delimiter is '|'
     * Example: |lte_osna_stadt_17082018_114759|b|12|
     */

    bool params_ok = false;
    char recvmsg[PACKETSIZE];
    vector<string> recv_params;
    while(not params_ok)    {

        memset(recvmsg,0, sizeof(recvmsg));
        if (0 > read( *tcp_sock , recvmsg, sizeof(recvmsg))) // this is a blocking call
        {
            printf("tcp_recvMeasurmentParams - error: %i\n", errno);
            tcp_closeConnection(tcp_sock);
            return -1;
        }

        vector<string> recv_params = split(recvmsg,'|');

        //for(std::vector<string>::iterator it = recv_params.begin(); it != recv_params.end(); ++it) {
        //   cout << *it << endl;
        //}

        //TODO: check measurementId (must be in format [TECH_TOWN_AREA_DATE_TIME]
        //TODO: check direction (must be 'b' or 'u'
        //TODO: check Timeout (must be int number)

        params_ok = true;


        if (params_ok)
        {
            for (int i=0; i<recv_params.size(); i++)
                (*checked_params).push_back(recv_params[i]);

            char ackMsg[PACKETSIZE] = "ack";
            if (0 > send(*tcp_sock , ackMsg , strlen(ackMsg) , 0 ))
            {
                printf("tcp_recvMeasurmentParams - error: %i\n", errno);
                tcp_closeConnection(tcp_sock);
                return -1;
            }
            return 0;

        } else {
            // if params are incorrect, deny measurement and wait for new params
            char ackMsg[PACKETSIZE] = "denied";
            if (0 > send(*tcp_sock , ackMsg , strlen(ackMsg) , 0 ))
            {
                printf("tcp_recvMeasurmentParams - error: %i\n", errno);
                tcp_closeConnection(tcp_sock);
                return -1;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if(argc < 4){
        cout << "Usage: ./Server [TCP-Port] [UDP-Port] [device]" << endl;
        // Example ./Server 8080 8081 lo
        return -1;
    }

    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);
    char* local_dev = argv[3];

    /* init tcp connection */
    int* tcp_sock = (int *) malloc(sizeof(int));
    if (0 > tcp_initializeConnectionServer(tcp_sock, tcp_port))
    {
        printf("tcp_initializeConnectionServer - error: %i\n", errno);
        tcp_closeConnection(tcp_sock);
        return -1;
    }
    printf("TCP socket was created successfully\n");
    vector<string>* params = new vector<string>(0);
    if (0 > tcp_recvMeasurementParameters(tcp_sock, params))
    {
        printf("tcp_recvMeasurementParameters - error: %i\n", errno);
        tcp_closeConnection(tcp_sock);
        return -1;
    }
    printf("Received measurement parameters successfully\n");
/*
    for(vector<string>::iterator it = (*params).begin(); it != (*params).end(); ++it) {
        cout << *it << endl;
    }
*/
    string measurementid = params->at(1);




//    char direction = (params->at(2)).c_str()[0];
//    int timeoutServer = atoi((params->at(3)).c_str());

      /* make writeable directory at SERVER_LOCATION */
      string dirpath = SERVER_LOCATION;
      dirpath = dirpath + "/" + measurementid;
      if (0 > makeDirectoryForMeasurement(dirpath))
      {
          printf("makeDirectoryForMeasurement - error: directory creation failed\n");
          tcp_closeConnection(tcp_sock);
          return -1;
      }
      printf("Creation of measurement directory successfull (%s)\n", dirpath.c_str());

      pid_t child_pcap = fork();
      if (child_pcap == 0)
      {
          /* child does not have to listen what the tcp_socket has to say ;) */
          tcp_closeConnection(tcp_sock);

          string pcapfile = dirpath;
          pcapfile = pcapfile + "/" + measurementid + ".pcap";
          PcapWriter *writer = new PcapWriter();
          writer->start(local_dev, stringToChar(pcapfile)); /* some threads are started here, the will be closed by SIGINT */
      } else {

                iperf_generateLoadServer(udp_port,1);

//            /*
//             * clean up the processes, threads, mallocs,...
//             * udp_sock is closed by timeout in send and recv which results closing the udp_socket
//             */
              kill_child_end_tcp(child_pcap, tcp_sock);
      }

      return 0;
}
