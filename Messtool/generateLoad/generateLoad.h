/**
 * This class is container for functions which generate load
 */

#ifndef MESSTOOL_GENERATELOAD_H
#define MESSTOOL_GENERATELOAD_H

#include "../consts_and_utils/consts_and_utils.h" /* for packetsize constant*/
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h> // sleep functions
#include <cstdint>
#include <cstring>

/**
 * A function which generates a CBR load of a specifed duration via a udp link.
 * @param sock stores socket data
 * @param dest stores the destination addr
 * @param packetSize the packetsize for the load generation
 * @param load the load measured in 10^6 Bits (similar to netperf)
 * @param sec time which the load should be simulated, measured in seconds to simulate load
 * @return -1 if some error occured
 */
int udp_generateLoad(int* sock, struct sockaddr_in * dest, char* measurementid, double load, int sec);

int iperf_generateLoadServer(int port, int intervall);

int iperf_generateLoadClient(string ipDest, int port, int bandwidth, char bw_unit);


#endif //MESSTOOL_GENERATELOAD_H
