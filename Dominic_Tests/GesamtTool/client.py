import os
import socket
import threading
from threading import Thread

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
        value = speed*1000
    elif entity == "Kbits/sec":
        value = speed/1000
    elif entity == "Mbits/sec":
        value = speed
    elif entity == "Tbits/sec":
        value = speed*1000000
    else:
        value = "No valid measurement result!"
    return(value)

if __name__ == "__main__":
    threads = []
    t1 = Thread(target=download, args=())
    t2 = Thread(target=upload, args=())
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:  # Wait till all threads are finished
        thread.join()

    destIP = "127.0.0.1"
    destPort = 5000
    beginMessage = "Start"
    stopMessage = "Stop"

    ####################################################################################################################
    # socket handling
    ####################################################################################################################
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    tcpsock.send(stopMessage.encode())
    print("hier")
    uploadLine = readFile("upload.txt")
    downloadLine = readFile("download.txt")
    uploadspeed = calcBandwidthResult(uploadLine)
    downloadspeed = calcBandwidthResult(downloadLine)
    print("Upload: {} Mbit/s".format(uploadspeed))
    print("Download: {} Mbit/s".format(downloadspeed))