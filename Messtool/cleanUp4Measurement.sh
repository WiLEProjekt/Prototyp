#! /bin/bash

#delete netperf
sudo pkill netserver
sudo pkill netperf

# delete iperf
sudo pkill iperf

#delete pcap
sudo pkill pcap

# delete Client and Server stuff
sudo pkill Client
sudo pkill Server

#TODO:  test this script
# see shared memory: ipcs
# delete shared memory segments: ipcrm

# find all processes of port 8080
# lsof -i :8080
