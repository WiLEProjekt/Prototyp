import socket
import time
from threading import Thread
import threading


def sendudp(killevent, socket, address, message):
    while not killevent.is_set():
        socket.sendto(message.encode(), address)


def tcphandling(killevent, socket):
    socket.listen(1)
    conn, addrtcp = socket.accept()
    datatcp = conn.recv(2048)
    # print(datatcp.decode())
    killevent.set()


if __name__ == "__main__":
    IP = "127.0.0.1"
    UDP_PORT = 5005
    TCP_PORT = 5006
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.bind((IP, UDP_PORT))

    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.bind((IP, TCP_PORT))

    dataudp, addrudp = udpsock.recvfrom(2048)
    counter = 0
    starttime = time.time()
    while counter < 15000:
        dataudp2, addrudp2 = udpsock.recvfrom(2048)
        counter += 1
    endtime = time.time()
    bandwidth = (((counter + 1) * 1400 * 8) / (endtime - starttime)) / 1024  # kbit/s

    # Response String zusammensetzen
    bandwidtstr = str(int(bandwidth))  # bandbreite auf 1kbit/s abrunden und zu string machen
    fillZeros = 1400 - len(bandwidtstr)
    downMessage = "0" * fillZeros
    downMessage += bandwidtstr

    threads = []
    killevent = threading.Event()
    t1 = Thread(target=sendudp, args=(killevent, udpsock, addrudp, downMessage))
    t2 = Thread(target=tcphandling, args=(killevent, tcpsock))
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:
        thread.join()
