//
// Created by drieke on 09.08.18.
//


#include "measureBandwidth.h"
#include "PcapWriter.h"

int main(int argc, char **argv) {

    //Parameter:
    // Server-IP
    // Schnittstelle (bsp: enp0s31f6)
    // Speicherort/Technologie_Region_Ort

    // Blas
    //- folder name: Technologie_Region_Ort_Datum_Uhrzeit
    // filenames: bandwidth.txt, signalstrength.txt, load.pcap

    /* get signal strength parameters */
    //TODO:

    /* measure bandwidth with saturated load */
    const char* filename = "bandwidthTextfile"; /* sollte char* bleiben */
    measureThroughput(filename);

    /* write pcap */
    PcapWriter *writer = new PcapWriter();
    writer->start("enp0s31f6", "load.pcap"); //TODO: enp0s irgendwas setzen (siehe ifconfig)

    /* generate load*/
    //TODO:

}
