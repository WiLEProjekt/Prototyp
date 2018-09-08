import socket
import threading
import time
from threading import Thread

def sendudp(killevent, socket, ip, port):
    udp_message = "a"*1400
    while not killevent.is_set():
        socket.sendto(udp_message.encode(), (ip, port))

def udpreceive(killevent, udpsocket, tcpsocket):
    data, addr = udpsocket.recvfrom(2048)
    print("Upload Speed: {} kbit/s".format(int(data)))
    killevent.set()
    counter = 0
    starttime = time.time()
    while counter < 15000:
        dataudp2, addrudp2 = udpsocket.recvfrom(2048)
        counter += 1
    endtime = time.time()
    bandwidth = (((counter + 1) * 1400 * 8) / (endtime - starttime)) / 1024  # kbit/s

    print("Download Speed: {} kbit/s".format(int(bandwidth)))
    TCP_Message = "END"
    tcpsock.connect((IP, TCP_PORT))
    tcpsocket.send(TCP_Message.encode())

if __name__ == "__main__":
    IP = "131.173.33.211"
    UDP_PORT = 5005
    TCP_PORT = 5006

    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP


    threads = []
    killevent = threading.Event()
    t1 = Thread(target=sendudp, args=(killevent, udpsock, IP, UDP_PORT))
    t2 = Thread(target=udpreceive, args=(killevent, udpsock, tcpsock))
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:
        thread.join()