import socket
import threading
from threading import Thread
import subprocess
import time
import os

def bandwidtUpload(): #open iperf server process for upload measurement
    sub1 = subprocess.Popen("iperf -c 127.0.0.1 -p 5001 > cupload.txt", shell=True)

def bandwidthDownload(): #open iperf server process for download measurement
    sub2 = subprocess.Popen("iperf -c 127.0.0.1 -p 5002 --reverse > cdownload.txt", shell=True)


if __name__ == "__main__":
    destIP = "127.0.0.1"
    destPort = 5000
    beginMessage = "Start"
    stopMessage = "Stop"

    ####
    #socket handling
    ####
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    tcpsock.send(beginMessage.encode())
    time.sleep(2) # to assure that the server opened both iperf servers

    threads = []
    killevent = threading.Event()
    t1 = Thread(target=bandwidtUpload, args=())
    t2 = Thread(target=bandwidthDownload, args=())
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:  # Wait till all threads are finished
        thread.join()
    tcpsock.send(stopMessage.encode())
    tcpsock.close()