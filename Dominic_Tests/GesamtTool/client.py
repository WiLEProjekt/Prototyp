import os
import socket
import threading
from threading import Thread
import time

def download():
    os.system("iperf -c 127.0.0.1 -p 5001 --reverse > download.txt")

def upload():
    os.system("iperf -c 127.0.0.1 -p 5002 > upload.txt")

def readFile(filename):
    file = open(filename, "r")
    fileLines = file.readlines()
    lastLine = fileLines[-1]
    return(lastLine)

def calcBandwidthResult(lastFileLine):
    print(lastFileLine)
    speed = float(lastFileLine[-16:-10]) #fetch result and convert it to a float
    entity = lastFileLine[-10:-1] #fetch entity in string format
    if entity == "Gbits/sec":
        value = speed*1000000
    elif entity == "Kbits/sec":
        value = speed
    elif entity == "Mbits/sec":
        value = speed*1000
    elif entity == "Tbits/sec":
        value = speed*1000000000
    else:
        value = "No valid measurement result!"
    return(value)

if __name__ == "__main__":

    destIP = "127.0.0.1"
    destPort = 5000
    startMessage = "Start"
    stopMessage = "Stop"
    udpPort = 5010


    # socket handling
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    tcpsock.connect((destIP, destPort))
    tcpsock.send(startMessage.encode())

    time.sleep(2)
    threads = []
    t1 = Thread(target=download, args=())
    t2 = Thread(target=upload, args=())
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:  # Wait till all threads are finished
        thread.join()

    tcpsock.send(stopMessage.encode()) # Signal Server that bandwidth-measurement has finished

    uploadLine = readFile("upload.txt")
    downloadLine = readFile("download.txt")
    uploadspeed = calcBandwidthResult(uploadLine)
    downloadspeed = calcBandwidthResult(downloadLine)
    print("Upload: {} Kbit/s".format(uploadspeed))
    print("Download: {} Kbit/s".format(downloadspeed))

    # CBR
    print("Starting CBR-Test")
    cbrspeedkbits = int(round((min(uploadspeed, downloadspeed)/2), 0)) #round to int
    cbrspeedbytes = int(round((cbrspeedkbits*1000/8),0))
    packetsize = 1000 #byte
    packetssec = int(round((cbrspeedbytes/packetsize),0))
    print(packetssec)
    time.wait(2) # wait to assure that the server has closed its threads
    tcpsock.send(str(packetssec).encode())
    udpsock.sendto("CBRUDP".encode(), (destIP, udpPort))
    tcpsock.settimeout(1) #1sec timeout
    loopexit = False
    while not loopexit:
        try:
            answer = tcpsock.recv(2048)
            if answer.decode() == "ACK":
                loopexit = True
        except socket.timeout:
            loopexit = False


    tcpsock.close()
