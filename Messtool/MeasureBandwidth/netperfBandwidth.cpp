/**
 * This class contains a throughput measurement based on netperf.
 */

#include "netperfBandwidth.h"
#include "../consts_and_utils/consts_and_utils.h"



int measureThroughputViaUDP_Stream(const char *filename, string netPerf, double* upload_bw, double* download_bw)
{
    std::ifstream infile;
    std::ofstream outfile;
    string throughput_send = "-1";
    string throughput_recv = "-1";
    std::string line;
    vector<string> substrings;

    const char* c = netPerf.c_str();
    if (0 < system(c))
    {
        perror("Failure at starting netperf \n check netperf call\n");
        return -1;
    };


    infile.open("netperf_bandwidth_logger.txt");
    while (getline(infile,line))
    {
        istringstream iss(line);
        do
        {
            string subs;
            iss >> subs;
            substrings.push_back(subs);
        } while (iss);
    }
    infile.close();

    if ( is_positiv_number(substrings[substrings.size() - 3])
         && is_positiv_number(substrings[substrings.size() - 8]) )
    {
        throughput_send = substrings[substrings.size() - 3 ];
        throughput_recv =substrings[substrings.size() - 8];
        *download_bw = atof(throughput_recv.c_str());
        *upload_bw = atof(throughput_send.c_str());
    } else {
        puts("Extracting throughput failed");
        return -1;
    }

    /* delete netperf_bandwidth_logger.txt */
    //if (remove("netperf_bandwidth_logger.txt") != 0)
    //	perror("Error deleting netperf_bandwidth_logger.txt\n");

    /* create output textfile */
    //printf("filename: %s\n", filename);
    outfile.open(filename);
    outfile << "Throughput (send): " + throughput_recv + " 10^6bits/sec ";
    outfile << "Throughput (recv): " + throughput_send + " 10^6bits/sec\n";
    outfile.close();

    //puts("Throuput sucessfully written to file");

    return 0;
}




