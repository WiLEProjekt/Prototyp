import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests, xmltodict
from datetime import datetime
from threading import Thread
import threading

def sendStart(udpsock, message, ip, port):
    for i in range(10): #send the start message
        udpsock.sendto(message.encode(), (ip, port))

if __name__ == "__main__":
    argv = sys.argv[1:]
    destIP=""
    destPort=0
    packetsize=1000 #default 1000Byte
    bandwidth=1000 #default 1Mbit/s
    try:
        opts, args = getopt.getopt(argv, "hi:p:b:", ["destionation IP", "destination Port", "bandwidth"])
    except getopt.GetoptError:
        print("Usage: python3 Client.py -i <dest IP> -p <dest Port> -b <bandwidth [kbit/s>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -i <dest IP> -p <dest Port> -b <bandwidth [kbit/s>")
            sys.exit()
        elif opt in ("-i", "--ip"):
            destIP = arg
        elif opt in ("-p", "--port"):
            destPort = int(arg)
        elif opt in ("-b", "--bandwidth"):
            bandwidth = int(arg)
    if destIP == "" or destPort is 0:
        print("No IP or no Port provided.")
        sys.exit(2)

    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.settimeout(2)
    Message=str(packetsize)+"_"+str(bandwidth)
    sendStart(udpsock, Message, destIP, destPort)
    while True:
        try:
            data, addr = udpsock.recvfrom(2048)
        except socket.timeout:
            print("connection lost, reset connection.")
            sendStart(udpsock, Message, destIP, destPort)