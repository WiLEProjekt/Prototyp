import multiprocessing
import os
import json
import pcapy
import signal
import sys


def signal_term_handler():
    sys.exit(0)


def write_pcap(filename, interface):
    pcapy.findalldevs()
    max_bytes = 2048
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live(interface, max_bytes, promiscuous, read_timeout)
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


def read_tcp_bandwidth(filename):
    file = open(filename, "r")
    json_file = json.load(file)
    bandwidth = json_file['end']['sum_received']['bits_per_second']
    file.close()
    return bandwidth


def read_udp_bandwidth(filename):
    file = open(filename, "r")
    json_file = json.load(file)
    file.close()
    return json_file["end"]["sum"]["bits_per_second"]


def write_result(filename, result):
    file = open(filename, "w")
    for x in result:
        file.write(x + ";")
    file.close()


def main():
    tcp_filename_down = "tcp.json"
    udp_filename_down = "udp.json"
    udp_plus_filename_down = "udppluss.json"
    udp_minus_filename_down = "udpminus.json"

    tcp_filename_up = "tcpup.json"
    udp_filename_up = "udpup.json"
    udp_plus_filename_up = "udpplussup.json"
    udp_minus_filename_up = "udpminusup.json"

    result_tcp = []
    result_udp = []
    result_udp_plus = []
    result_udp_minus = []

    result_tcp_up = []
    result_udp_up = []
    result_udp_plus_up = []
    result_udp_minus_up = []
    for i in range(10):
        try:
            os.remove(tcp_filename_down)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_down)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_down)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_down)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_up)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_up)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_up)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_up)
        except FileNotFoundError:
            pass

        os.system("iperf3 -c 131.173.33.228 -p 50000 -J --logfile " + tcp_filename_down)
        result_tcp.append(read_tcp_bandwidth(tcp_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i])) + " -l 1450 -J --logfile " + udp_filename_down)
        result_udp.append(read_udp_bandwidth(udp_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i] * 1.1)) + " -l 1450 -J --logfile " + udp_plus_filename_down)
        result_udp_plus.append(read_udp_bandwidth(udp_plus_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i] / 1.1)) + " -l 1450 -J --logfile " + udp_minus_filename_down)
        result_udp_minus.append(read_udp_bandwidth(udp_minus_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -J --logfile " + tcp_filename_up)
        result_tcp_up.append(read_tcp_bandwidth(tcp_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i])) + " -l 1450 -J --logfile " + udp_filename_up)
        result_udp_up.append(read_udp_bandwidth(udp_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i] * 1.1)) + " -l 1450 -J --logfile " + udp_plus_filename_up)
        result_udp_plus_up.append(read_udp_bandwidth(udp_plus_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i] / 1.1)) + " -l 1450 -J --logfile " + udp_minus_filename_up)
        result_udp_minus_up.append(read_udp_bandwidth(udp_minus_filename_up))

    write_result("tcp.csv", result_tcp)
    write_result("udp.csv", result_udp)
    write_result("udpPlus.csv", result_udp_plus)
    write_result("udpMinus.csv", result_udp_minus)
    write_result("tcpup.csv", result_tcp_up)
    write_result("udpup.csv", result_udp_up)
    write_result("udpPlusup.csv", result_udp_plus_up)
    write_result("udpMinusup.csv", result_udp_minus_up)


if __name__ == "__main__":
    interface = sys.argv[1]
    pcap_process = multiprocessing.Process(target=write_pcap, args=("iperf_log.pcap", interface))\
        .start()
    main()
    pcap_process.terminate()
