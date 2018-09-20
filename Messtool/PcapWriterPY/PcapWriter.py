import pcapy
import time
import threading


def write_pcap():
    pcapy.findalldevs()

    max_bytes = 1024
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live("enp0s3", max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open("out.pcap")
    #pc.setfilter('udp')

    def recv_pkts(hdr, data):
        dumper.dump(hdr, data)

    packet_limit = -1 #infinite
    pc.loop(packet_limit, recv_pkts)


if __name__ == "__main__":
    t = threading.Thread(target=write_pcap, args=())
    t.start()
    print("jojojo")
    time.sleep(5)
    print("ende")
    t.join()

