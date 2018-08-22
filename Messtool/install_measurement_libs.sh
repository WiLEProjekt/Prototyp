#! /bin/bash

#install some libraries
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt install netperf
sudo apt install iperf
sudo apt-get install openssh-server^C
sudo apt-get install libpcap0.8-dev
sudo apt install net-tools
sudo apt install nmap

# install some tools
sudo apt install valgrind
sudo apt-get install wireshark
sudo apt-get install git-core

# create folders
sudo mkdir /home/Measurements
sudo chmod +x /home/Measurements/
sudo mkdir /home/Measurements/Server
sudo chmod +x /home/Measurements/Server
sudo mkdir /home/Measurements/Client
sudo chmod +x /home/Measurements/Client




echo IMPORTANT: SET CONSTANTS IN FILE 'consts_and_utils.h' and create directories