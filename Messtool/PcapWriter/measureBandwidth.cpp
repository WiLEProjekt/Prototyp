//
// Created by drieke on 09.08.18.
//
#include "measureBandwidth.h"


bool is_positiv_number(string str)
{
    regex db("([[:digit:]]+)(\\.(([[:digit:]]+)?))?");
    if(regex_match(str,db))
        return true;
    else
        return false;
}

/* Simple function to measure the throughput based on netperf. Netperf is started and the output is logged.
* The troughput is written to a outputfile 'filename'. */

void measureThroughput(const char* filename)
{
    std::ifstream infile;
    std::ofstream outfile;
    string throughput_send = "-1";
    string throughput_recv = "-1";
    std::string line;
    vector<string> substrings;

    const char* c = "netperf -t UDP_STREAM -- -m 1024 >> netperf_bandwidth_logger.txt"; //TODO: start netem with correct param -> argv?
    system(c);


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
    } else {
        puts("Extracting throughput failed");
        goto end;
    }

    /* delete netperf_bandwidth_logger.txt */
    //if (remove("netperf_bandwidth_logger.txt") != 0)
    //	perror("Error deleting netperf_bandwidth_logger.txt\n");

    /* create output textfile */
    outfile.open(filename);
    outfile << "Throughput (send): " + throughput_recv + " 10^6bits/sec ";
    outfile << "Throughput (recv): " + throughput_send + " 10^6bits/sec\n";
    outfile.close();

    puts("Throuput sucessfully written to file");
    end:
    return ;
}




