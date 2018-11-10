import socket
from threading import Thread
import threading

def udphandling(killevent, ip, udp_port):
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    udpsock.bind((ip, udp_port))
    while not killevent.is_set():
        dataudp, addrudp = udpsock.recvfrom(1024) # buffer size is 1024 bytes
        print(dataudp.decode())

def tcphandling(killevent, ip, tcp_port):
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
    tcpsock.bind((ip, tcp_port))
    tcpsock.listen(1)
    conn, addrtcp = tcpsock.accept()
    datatcp = conn.recv(1024)
    print(datatcp.decode())
    killevent.set()

if __name__ == "__main__":
    IP = "131.173.33.211"
    UDP_PORT = 5005
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.bind((IP, UDP_PORT))
    while True:
        dataudp, addrudp = udpsock.recvfrom(2048)  # buffer size is 1024 bytes
        print(dataudp.decode())
        udpsock.sendto(dataudp, addrudp)
        udpsock.sendto(dataudp, addrudp)

