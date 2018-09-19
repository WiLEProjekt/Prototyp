import os
import socket
import threading
from threading import Thread

def iperfServerReverse(): #closes automatically
    os.system("iperf -s -p 5001 --reverse")

def iperfServer(): #does not close automatically -> client sends stop message when measurement is finished and other thread kills iperf so this thread finishes execution
    os.system("iperf -s -p 5002")

def tcpreceive():
    datatcp = conn.recv(2048).decode()
    if datatcp == "Stop":
        os.system("pkill iperf")

if __name__ == "__main__":
    myIP = "127.0.0.1"
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.bind((myIP, 5000))
    tcpsock.listen()
    conn, addrtcp = tcpsock.accept()
    datatcp = conn.recv(2048).decode()
    if datatcp == "Start":
        threads = []
        t1 = Thread(target=iperfServerReverse, args=())
        t2 = Thread(target=iperfServer, args=())
        t3 = Thread(target=tcpreceive, args=())
        threads.append(t1)
        threads.append(t2)
        threads.append(t3)
        t1.start()
        t2.start()
        t3.start()
        for thread in threads:  # Wait till all threads are finished
            thread.join()
        print("hier")
        udppackets = conn.recv(2048).decode()
        print(udppackets)
    else:
        print("No start-message received")
        tcpsock.close()