import socket
import threading
from threading import Thread
import subprocess

def bandwidtUpload(): #open iperf server process for upload measurement
    sub1 = subprocess.Popen(['iperf', '-s', '-p 5001'], stdout=subprocess.PIPE)
    upOutput = sub1.communicate()[0].decode()
    print(upOutput)

def bandwidthDownload(): #open iperf server process for download measurement
    sub2 = subprocess.Popen(['iperf', '-s', '-p 5002', '--reverse'], stdout=subprocess.PIPE)
    downOutput = sub2.communicate()[0].decode()
    print(downOutput)

if __name__ == "__main__":
    myIP = "127.0.0.1"
    destPort = "5000"
    beginMessage = "Start"

    ####
    #socket handling
    ####
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.bind((myIP, 5000))
    tcpsock.listen()
    conn, addrtcp = tcpsock.accept()
    datatcp = conn.recv(2048).decode()
    if datatcp == "Start":
        threads = []
        killevent = threading.Event()
        t1 = Thread(target=bandwidtUpload, args=())
        t2 = Thread(target=bandwidthDownload, args=())
        threads.append(t1)
        threads.append(t2)
        t1.start()
        t2.start()
        for thread in threads: #Wait till all threads are finished
            thread.join()
    else:
        print("Received wrong message, waiting for new connection!")
