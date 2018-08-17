
#include "generateLoad.h"


int udp_generateLoad(int* sock, struct sockaddr_in * dest, char* measurementid, double load, int sec)
{
    /*
     * TODO: zu lösende Probleme
     * 1. Die sleep-Funktionen in c++ sind unterschiedlich genau. nanosleep scheint verhältnismäßig präzise zu sein,
     * allerdings solle dies separat überprüft / getestet werden und durch Literatur am Besten nachgewiesen werden.
     * 2. Durch Messungenauigkeiten und Rundungen wird unter Umständen nicht die tatsächlich angegebene Last emuliert.
     */

    int numberOfPackets = ((load / 8) * 1000000) / PACKETSIZE;// TODO: precision issue because of float point
    struct timespec timePauseBetween2Packets;
    timePauseBetween2Packets.tv_sec = 0;
    timePauseBetween2Packets.tv_nsec =  (1000000000 / numberOfPackets);

    //cout << "numberofpackets: " << numberOfPackets << endl;
    //cout << "time between two packets: " << ts_sleep.tv_nsec << endl;

    short seqnr = 0;
    int passedSeconds = 0;

    /* generate for 'sec' seconds a load of 'load' 10^6 Bits per second */
    while(sec >passedSeconds)
    {
        /* generate the load per second */
        for (int i = 0; i < numberOfPackets ; i++)
        {
            unsigned char data[PACKETSIZE];
            memset(data,0, sizeof(data));
            memcpy(data,&seqnr,sizeof(seqnr));
            memcpy(data+2,measurementid,strlen(measurementid));
            int snd = sendto(*sock, data, PACKETSIZE, 0, (struct sockaddr*) dest, sizeof (*dest));
            if (0 > snd)
            {
                return -1;
            }
            seqnr++;

            /* sleep after sending each packet */
            nanosleep(&timePauseBetween2Packets, NULL); //TODO: how precise is this?
        }
        passedSeconds++;
    }

    return 0;
}

