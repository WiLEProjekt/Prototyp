import pcapy
import time
import multiprocessing
import signal
import sys


def signal_term_handler(signal, frame):
    sys.exit(0)


def write_pcap():
    pcapy.findalldevs()

    max_bytes = 1024
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live("enp0s31f6", max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open("out.pcap")
    signal.signal(signal.SIGTERM, signal_term_handler)

    try:
        pc.setfilter('udp')

        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)

        packet_limit = -1 #infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()
        dumper.close()


if __name__ == "__main__":
    #Beispielhafter Aufruf
    pcap_process = multiprocessing.Process(target=write_pcap, args=())
    pcap_process.start()
    time.sleep(5)
    pcap_process.terminate()

