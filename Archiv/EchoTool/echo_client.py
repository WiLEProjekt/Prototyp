import socket

if __name__ == "__main__":
    IP = "131.173.33.211"
    UDP_PORT = 5005
    MESSAGE_UDP = "Hello, World!"

    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    udpsock.settimeout(5)
    udpsock.sendto(MESSAGE_UDP.encode(), (IP, UDP_PORT))
    while True:
        dataudp, addrudp = udpsock.recvfrom(2048)
        print(dataudp.decode())