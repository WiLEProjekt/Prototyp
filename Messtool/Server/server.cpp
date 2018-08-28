#include "../generateLoad/generateLoad.h"
#include "../consts_and_utils/consts_and_utils.h"
#include "../generateLoad/generateLoad.h"
#include <../PcapWriter/PcapWriter.h>

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
#include <sstream>
#include <iterator>
#include <regex>
#include <mutex>
#include <sys/shm.h>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

/**
 * Creates a tco connection for a specific port and stores data in tcp_sock
 * @param tcp_sock the tcp socket
 * @param port the tcp port
 * @return -1 if an error occur, else 0
 */

int tcp_initializeConnectionServer(int *tcp_sock, int port) {
    //Open Socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
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
    if (0 > bind(sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in))) {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    // listen for connections
    if (listen(sock, 3) < 0) {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    //accepts connection
    int addrlen = sizeof(addr);
    if ((*tcp_sock = accept(sock, (struct sockaddr *) &addr,
                            (socklen_t *) &addrlen)) < 0) {
        printf("tcp_initializeConnectionServer - Errno: %i\n", errno);
        return -1;
    }
    return 0;
}

/**
 * Free the tcp socket without closing socket before.
 * @param tcp_sock the tcp socket
 */

void tcp_closeConnection(int *tcp_sock) {
    free(tcp_sock);
}

/**
 * Close tcp socket, free tcp pointer and kill a process child_pid
 * @param child_pid the child pid
 * @param tcp_sock the tcp socket
 */

void kill_childs_end_tcp(pid_t child_pid, int *tcp_sock) {
    close(*tcp_sock);
    free(tcp_sock);
    kill(child_pid, SIGINT); /* ends PcapWriter Process and its threads*/
    string pkill= "sudo pkill iperf";  /* ends iperf Process*/
    system(pkill.c_str());
}

/**
 * Receive some measurement parameters via tcp. PArameters are checked and then saved into check_params. Return
 * -1 if parameters are invalid or some other error occured.
 * @param tcp_sock the tcp socket
 * @param checked_params a vector for the parameters
 * @return -1 if parameters are invalid or soem other error occured
 */

int tcp_recvMeasurementParameters(int *tcp_sock, vector<string> *checked_params) {
    /*
     * Parameter: |[measurementId]|
     * Delimiter is '|'
     * Example: |lte_osna_stadt_17082018_114759|
     */

    bool params_ok = false;
    char recvmsg[PACKETSIZE];
    vector<string> recv_params;
    while (not params_ok) {

        memset(recvmsg, 0, sizeof(recvmsg));
        if (0 > read(*tcp_sock, recvmsg, sizeof(recvmsg))) // this is a blocking call
        {
            printf("tcp_recvMeasurmentParams - error: %i\n", errno);
            tcp_closeConnection(tcp_sock);
            return -1;
        }

        vector<string> recv_params = split(recvmsg, '|');

        //for(std::vector<string>::iterator it = recv_params.begin(); it != recv_params.end(); ++it) {
        //   cout << *it << endl;
        //}

        //TODO: check measurementId (must be in format [TECH_TOWN_AREA_DATE_TIME]
        params_ok = true;

        if (params_ok) {
            for (int i = 0; i < recv_params.size(); i++)
                (*checked_params).push_back(recv_params[i]);

            char ackMsg[PACKETSIZE] = "ack";
            if (0 > send(*tcp_sock, ackMsg, strlen(ackMsg), 0)) {
                printf("tcp_recvMeasurmentParams - error: %i\n", errno);
                tcp_closeConnection(tcp_sock);
                return -1;
            }
            return 0;

        } else {
            // if params are incorrect, deny measurement and wait for new params
            char ackMsg[PACKETSIZE] = "denied";
            if (0 > send(*tcp_sock, ackMsg, strlen(ackMsg), 0)) {
                printf("tcp_recvMeasurmentParams - error: %i\n", errno);
                tcp_closeConnection(tcp_sock);
                return -1;
            }
        }
    }
}

/**
 * Signal a server registered on a tcp socket to close connection and end or free its processes, threads and mallocs gracefully.
 * @param tcp_sock the tcp connection
 * @return -1 if some error occured, else 0
 */

int tcp_recvSignalServerToCleanUp(int *tcp_sock) {
    bool params_ok = false;
    char recvmsg[PACKETSIZE];
    vector<string> recv_params;
    memset(recvmsg, 0, sizeof(recvmsg));
    if (0 > read(*tcp_sock, recvmsg, sizeof(recvmsg))) // this is a blocking call
    {
        printf("tcp_recvSignalServerToCleanUp - error: %i\n", errno);
        tcp_closeConnection(tcp_sock);
        return -1;
    }
    printf("recv Msg: %s\n", recvmsg);
    return 0;
}


int main(int argc, char **argv) {
    int tcp_port;
    int udp_port;
    string device;
    options_description desc{"Options"};
    desc.add_options()
            ("help,h", "Help screen")
            ("porttcp,t", value<int>(&tcp_port), "TCP-Port of the Server")
            ("portudp,u", value<int>(&udp_port), "UDP-Port of the Server")
            ("device,e", value<string>(&device), "Name of the networkdevice of your computer");
    try{
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        if(vm.count("help")){
            cout << desc << endl;
            return 0;
        }else if(!(vm.count("porttcp") && vm.count("portudp") && vm.count("device"))){
            cout << "Missing parameters. All parameters need to be provided." << endl;
            return -1;
        }
    }catch(const error &ex){
        cerr << ex.what() << endl;
    }
    char* local_dev = new char[device.length() + 1]; /* only on client computer valid */
    strcpy(local_dev, device.c_str());

    /* init tcp connection */
    int *tcp_sock = (int *) malloc(sizeof(int));
    if (0 > tcp_initializeConnectionServer(tcp_sock, tcp_port)) {
        printf("tcp_initializeConnectionServer - error: %i\n", errno);
        tcp_closeConnection(tcp_sock);
        return -1;
    }
    printf("TCP socket was created successfully\n");
    vector<string> *params = new vector<string>(0);
    if (0 > tcp_recvMeasurementParameters(tcp_sock, params)) {
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

    /* make writeable directory at SERVER_LOCATION */
    string dirpath = SERVER_LOCATION;
    dirpath = dirpath + "/" + measurementid;
    if (0 > makeDirectoryForMeasurement(dirpath)) {
        printf("makeDirectoryForMeasurement - error: directory creation failed\n");
        tcp_closeConnection(tcp_sock);
        return -1;
    }
    printf("Creation of measurement directory successfull (%s)\n", dirpath.c_str());

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
    if (child_pcap == 0) {
        /* child does not have to listen what the tcp_socket has to say ;) */
        tcp_closeConnection(tcp_sock);

        string pcapfile = dirpath;
        pcapfile = pcapfile + "/" + measurementid + ".pcap";
        PcapWriter *writer = new PcapWriter();
        (*mt_ptr).unlock(); // child has started already
        writer->start(local_dev, stringToChar(pcapfile)); /* some threads are started here, the will be closed by SIGINT */
    } else {

        /* wait until child unlocks the mutex in the shm */
        while (!(*mt_ptr).try_lock())
        {
            sleep(1);
        }

        /* because iperf_generateLoadServer is a blocking call a child process is created */
        pid_t child_iperf = fork();
        if (child_iperf == 0) {
            /* child does not have to listen what the tcp_socket has to say ;) */
            tcp_closeConnection(tcp_sock);

            /* start server to receive load */
            (*mt_ptr).unlock();
            iperf_generateLoadServer(udp_port, 1); //TODO: brauchen wir intervall 1?
        } else {

            /* wait until child unlocks the mutex in the shm */
            while (!(*mt_ptr).try_lock())
            {
                sleep(1);
            }
            shmdt(mt_ptr); // detach pointer from shm
            shmctl(shmid,IPC_RMID, NULL); // release shm

            if (0 > tcp_recvSignalServerToCleanUp(tcp_sock)) // this is ablocking call
            {
                return -1;
            }
            printf("Recevied signal to close connections and clean up\n");

            /*
           * clean up the processes, threads, mallocs,...
           * udp_sock is closed by timeout in send and recv which results closing the udp_socket
           */
            kill_childs_end_tcp(child_pcap, tcp_sock);
            printf("Measurment completed successfully\n");
        }
    }

    return 0;
}
