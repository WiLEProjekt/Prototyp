
/*
 * This header contains some constant values and util functions which are used in client and server to make live easier.
 */

#ifndef MESSTOOL_MEASUREMENT_CONSTS_H
#define MESSTOOL_MEASUREMENT_CONSTS_H

#include <string>
#include <cstring>
#include <sys/stat.h> // exist directory test
#include <vector>
#include <sstream> // split-function
#include <regex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define PACKETSIZE 1440 //Brauchen wir nicht mehr!!!
#define CLIENT_LOCATION "/home/Measurements/Client"
#define SERVER_LOCATION "/home/Measurements/Server"

/**
 * Create a directory at a specific path 'dirpath'.
 * @param dirpath the directory path
 * @return 0 if sucessfull, else -1
 */
int makeDirectoryForMeasurement(string dirpath);

/**
 * It's a bad joke that a standard string to char function (that does not result in 'const char')
 * does not exists.
 * @param str
 * @return
 */
char* stringToChar(string str);

/**
 * Splits a string at a defined delimiter and stores strings in a vector.
 * @param s the string which will be splitted by delimiter
 * @param delimiter the delimiter as a simple char
 * @return a vector containing a sequence of strings
 */
vector<string> split(const string& s, char delimiter);

/**
 * Assure that written a string a float point number.
 * @param str the string which will be checked
 * @return true if string resembles float point number, else false
 */
bool is_positiv_number(string str);

/**
 * A function receives packets via udp and echo it back the sender.
 * @param udp_sock the udp_socket
 * @return
 */

int udp_recvAndEcho(int *udp_sock);

/**
 * Setup the udp socket and save setup data into the pointers sock and dest.
 * @param sock stores the udp-socket
 * @param port the destination port as integer
 * @param timeoutServer the timeout of the socket (in seconds)
 * @return if some error occure a -1 is returned
 */
int udp_setupConnectionServer(int *sock, int port, int timeoutServer);

/**
 * Setup the udp socket and save setup data into the pointers sock and dest.
 * @param udp_sock stores the udp-socket
 * @param udp_dest stores the address of the destination
 * @param destIp the destination ip as string
 * @param port the destination port as integer
 * @return if some error occure a -1 is returnd
 */
int udp_setupConnectionClient(int * udp_sock, struct sockaddr_in * udp_dest, string destIp, int port, int udp_socket_timeout);

/**
 * free used udp pointer (socket will NOT be closed before freeing).
 * @param sock the udp-socket
 * @param dest the destination address (prob.ly some server)
 */

void udp_freePointer(int* sock, struct sockaddr_in * dest);

/**
 * Listen for data at a specific udp_socket until the socket-timeout cancels connection and free resources.
 * @param udp_sock the udp-socket
 * @return 0, if udp_sock is closed by timeout, else -1
 */

int udp_listen4data(int* udp_sock);

/**
 * Free udp socket without close without closing socket before.
 * @param udp_sock the udp socket
 */

void udp_closeConnection(int *udp_sock);

#endif //MESSTOOL_MEASUREMENT_CONSTS_H
