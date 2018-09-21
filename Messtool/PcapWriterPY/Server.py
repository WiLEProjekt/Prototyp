import socket
import pcapy
import time
import multiprocessing
import signal
import sys

BUFFER_SIZE = 64


def signal_term_handler(signal, frame):
    sys.exit(0)


def write_pcap(filename):
    pcapy.findalldevs()

    max_bytes = 1024
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live("enp0s31f6", max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open(filename + ".pcap")
    signal.signal(signal.SIGTERM, signal_term_handler)

    try:
        pc.setfilter('udp')

        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)

        packet_limit = -1 #infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()


def recieve():
    data = conn.recv(BUFFER_SIZE)
    if data == "stop":
        

def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((ip, port))

    while True:
        sock.listen(1)
        conn, addr = sock.accept()
        data = conn.recv(BUFFER_SIZE)
        pcap_process = multiprocessing.Process(target=write_pcap, args=(data.decode(),))
        pcap_process.start()
        tut = multiprocessing.Process(target=recieve, args=())
        tut.start()
        pcap_process.terminate()


if __name__ == "__main__":
    main("127.0.0.1", 84)
