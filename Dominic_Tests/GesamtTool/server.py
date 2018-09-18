import socket
import threading
from threading import Thread
import subprocess
from subprocess import check_output
import time
import os

def bandwidtUpload(): #open iperf server process for upload measurement
    sub1 = subprocess.Popen(['iperf', '-s', '-p 5001'], stdout=subprocess.PIPE)
    upOutput = sub1.communicate()[0].decode()

def bandwidthDownload(): #open iperf server process for download measurement
    sub2 = subprocess.Popen(['iperf', '-s', '-p 5002', '--reverse'], stdout=subprocess.PIPE)
    downOutput = sub2.communicate()[0].decode()

def tcpreceive(connection, address): #2. argument needed cuz of python specialities
    data = connection.recv(2048).decode()
    if data == "Stop":
        os.system("pkill iperf")

if __name__ == "__main__":
    myIP = "127.0.0.1"
    destPort = "5000"
    beginMessage = "Start"

    ####
    #socket handling
    ####
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.bind((myIP, 5000))
    while True:
        tcpsock.listen()
        conn, addrtcp = tcpsock.accept()
        datatcp = conn.recv(2048).decode()
        if datatcp == "Start":
            threads = []
            killevent = threading.Event()
            t1 = Thread(target=bandwidtUpload, args=())
            t2 = Thread(target=bandwidthDownload, args=())
            t3 = Thread(target=tcpreceive, args=(conn, addrtcp))
            threads.append(t1)
            threads.append(t2)
            threads.append(t3)
            t1.start()
            t2.start()
            t3.start()
            for thread in threads: #Wait till all threads are finished
                thread.join()
            conn.close()
            print("Waiting for new connection")
        else:
            print("Waiting for new connection")
