

#include "consts_and_utils.h"

int makeDirectoryForMeasurement(string dirpath)
{
        string mkdir = "sudo mkdir ";
        mkdir = mkdir + dirpath;
        system(mkdir.c_str());
        string rw_permission = "sudo chmod 777 "; // set rw permission to folder
        rw_permission = rw_permission + CLIENT_LOCATION;
        system(rw_permission.c_str());

        struct stat info;
        if( stat( dirpath.c_str(), &info ) != 0 )
        {
        //no access case
        return -1;
        } else if( info.st_mode & S_IFDIR )
        {
        // is directory
        return 0;
        } else
        {
        // no directory case
        return -1;
    }
}


char* stringToChar(string str)
{
    char* cstr = &str[0u];
    return cstr;
}

vector<string> split(const string& s, char delimiter)
{
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter))
        {
                tokens.push_back(token);
        }
        return tokens;
}

bool is_positiv_number(string str)
{
    regex db("([[:digit:]]+)(\\.(([[:digit:]]+)?))?");
    if(regex_match(str,db))
        return true;
    else
        return false;
}

int udp_recvAndEcho(int *udp_sock) {
    struct sockaddr_in addr;
    socklen_t slen = (socklen_t) sizeof(addr);
    char buf[PACKETSIZE];

    //keep listening for data
    while (1) {
        if (0 > recvfrom(*udp_sock, buf, (size_t) (sizeof(buf)), 0, (sockaddr *) &addr, (__socklen_t *) &slen)) {
            if (11 == errno) {
                return 0;
            } else {
                printf("udp_recvAndEcho - Errno: %i\n", errno);
                return -1;
            }
        }
        if (sendto(*udp_sock, buf, PACKETSIZE, 0, (struct sockaddr *) &addr, slen) == -1) {
            if (11 == errno) {
                return 0;
            } else {
                printf("udp_recvAndEcho - Errno: %i\n", errno);
                return -1;
            }
        }
    }
}

int udp_setupConnectionServer(int *sock, int port, int timeoutServer) {
    //Open Socket
    *sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (*sock < 0) {
        printf("udp_setupConnection - Errno: %i\n", errno);
        return -1;
    }
    //set sockopt
    struct timeval timeout;
    timeout.tv_sec = timeoutServer;
    timeout.tv_usec = 0;
    if (0 > setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        if (11 == errno) {
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
    if (0 > bind(*sock, (struct sockaddr *) &own_addr, sizeof(struct sockaddr_in))) {
        printf("udp_setupConnection - Errno: %i\n", errno);
        return -1;
    }
    return 0;
}

int udp_setupConnectionClient(int * udp_sock, struct sockaddr_in * udp_dest, string destIp, int port, int udp_socket_timeout)
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

void udp_freePointer(int* sock, struct sockaddr_in * dest)
{
    free(sock);
    free(dest);
}

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
 * Free udp socket without close without closing socket before.
 * @param udp_sock the udp socket
 */

void udp_closeConnection(int *udp_sock) {
    free(udp_sock);
}