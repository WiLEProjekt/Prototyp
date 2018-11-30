import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests, xmltodict
from datetime import datetime
from threading import Thread
import threading

#Global Variables
sequencenumber = 0
port = 5000  # default port
packetsize = 0
bandwidth = 0
ip=""

def calcSequencenumber(sequencenumber):
    return(sequencenumber+1)

def sendCBR(udpsock, sendstart):
    print("send")
    global packetsize, bandwidth, ip, sequencenumber
    while True:
        while sendstart.is_set():

            sequenceString = str(sequencenumber)
            Message = "0" * (packetsize - len(sequenceString)) + sequenceString
            udpsock.sendto(Message.encode(), (ip, port))
            sequencenumber = calcSequencenumber(sequencenumber)

def receiveMSGS(udpsock, sendstart):
    print("rcv")
    global packetsize, ip,port,bandwidth
    while True:
        data, addr = udpsock.recvfrom(2048)
        print(data.decode(), addr)
        ip = addr[0]
        port= addr[1]
        datastr = data.decode()
        packetsize2, bandwidth = datastr.split('_')
        packetsize = int(packetsize2)
        sendstart.set()


if __name__ == "__main__":
    argv = sys.argv[1:]

    try:
        opts, args = getopt.getopt(argv, "hp:", ["Port"])
    except getopt.GetoptError:
        print("Usage: python3 Server.py -p <Port>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -p <Port>")
            sys.exit()
        elif opt in ("-p", "--port"):
            port = int(arg)

    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.bind(("131.173.33.229", 5000))
    sendstart = threading.Event()
    recthread = Thread(target=receiveMSGS, args=(udpsock, sendstart))
    sendthread = Thread(target=sendCBR, args=(udpsock, sendstart))
    recthread.start()
    sendthread.start()
    recthread.join()
    sendthread.join()