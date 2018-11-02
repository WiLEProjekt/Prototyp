import os
import socket, multiprocessing, pcapy, signal, sys, getopt
from threading import Thread


def signal_term_handler(signal, frame):
    sys.exit(0)


def write_pcap(filename, interface):
    pcapy.findalldevs()
    max_bytes = 1024
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live(interface, max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open(filename + ".pcap")
    signal.signal(signal.SIGTERM, signal_term_handler)

    try:
        pc.setfilter('udp')

        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)

        packet_limit = -1  # infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()


def tcpreceive(connection, a):
    try:
        data = connection.recv(2048)
    except socket.error:
        print("Connection closed by Client")


def main(argv):
    ################################
    # Terminal argument parser
    ################################
    interface = ''
    try:
        opts, args = getopt.getopt(argv, "hi:", ["interface"])
    except getopt.GetoptError:
        print("Usage: python3 Server.py -i <networkinterface")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -i <networkinterface")
            sys.exit()
        elif opt in ("-i", "--interface"):
            interface = arg

    myip = "131.173.33.228"
    myport = 50001
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        sock.bind((myip, myport))
        sock.listen()
        while True:
            conn, addr = sock.accept()
            data = conn.recv(2048)
            measurementID = data.decode()
            if not os.path.exists(measurementID.split('/')[0]):
                os.makedirs(measurementID.split('/')[0])
            pcapfilename = measurementID
            print(pcapfilename)
            pcap_process = multiprocessing.Process(target=write_pcap, args=(pcapfilename, interface))
            pcap_process.start()
            t1 = Thread(target=tcpreceive, args=(conn, 1))
            t1.start()
            t1.join()
            pcap_process.terminate()
    except KeyboardInterrupt:
        sock.close()
        sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])
