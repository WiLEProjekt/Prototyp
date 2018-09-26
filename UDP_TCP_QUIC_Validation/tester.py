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
        file.write(str(x) + ";")
    file.close()


def main():
    tcp_filename_u = "tcp.json"
    udp_filename_u = "udp.json"
    udp_plus_filename_u = "udppluss.json"
    udp_minus_filename_u = "udpminus.json"

    tcp_filename_d = "tcpup.json"
    udp_filename_d = "udpup.json"
    udp_plus_filename_d = "udpplussup.json"
    udp_minus_filename_d = "udpminusup.json"

    tcp_filename_u5 = "tcp5.json"
    udp_filename_u5 = "udp5.json"
    udp_plus_filename_u5 = "udppluss5.json"
    udp_minus_filename_u5 = "udpminus5.json"

    tcp_filename_d5 = "tcpup5.json"
    udp_filename_d5 = "udpup5.json"
    udp_plus_filename_d5 = "udpplussup5.json"
    udp_minus_filename_d5 = "udpminusup5.json"

    tcp_filename_u15 = "tcp15.json"
    udp_filename_u15 = "udp15.json"
    udp_plus_filename_u15 = "udppluss15.json"
    udp_minus_filename_u15 = "udpminus15.json"

    tcp_filename_d15 = "tcpup15.json"
    udp_filename_d15 = "udpup15.json"
    udp_plus_filename_d15 = "udpplussup15.json"
    udp_minus_filename_d15 = "udpminusup15.json"

    result_tcp_u = []
    result_udp_u = []
    result_udp_plus_u = []
    result_udp_minus_u = []

    result_tcp_d = []
    result_udp_d = []
    result_udp_plus_d = []
    result_udp_minus_d = []

    result_tcp5_u = []
    result_udp5_u = []
    result_udp_plus5_u = []
    result_udp_minus5_u = []

    result_tcp_5_d = []
    result_udp_5_d = []
    result_udp_plus_5_d = []
    result_udp_minus_5_d = []


    result_tcp15_u = []
    result_udp15_u = []
    result_udp_plus15_u = []
    result_udp_minus15_u = []

    result_tcp_15_d = []
    result_udp_15_d = []
    result_udp_plus_15_d = []
    result_udp_minus_15_d = []

    for i in range(10):
        try:
            os.remove(tcp_filename_u)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_u)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_u)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_u)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_d)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_d)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_d)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_d)
        except FileNotFoundError:
            pass

        try:
            os.remove(tcp_filename_u5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_u5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_u5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_u5)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_d5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_d5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_d5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_d5)
        except FileNotFoundError:
            pass

        try:
            os.remove(tcp_filename_u15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_u15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_u15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_u15)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_d15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_d15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_d15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_d15)
        except FileNotFoundError:
            pass

        os.system("iperf3 -c 131.173.33.228 -p 50000 -J --logfile " + tcp_filename_u)
        result_tcp_u.append(read_tcp_bandwidth(tcp_filename_u))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp_u[i])) + " -l 1400 -J --logfile " + udp_filename_u)
        result_udp_u.append(read_udp_bandwidth(udp_filename_u))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp_u[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_u)
        result_udp_plus_u.append(read_udp_bandwidth(udp_plus_filename_u))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp_u[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_u)
        result_udp_minus_u.append(read_udp_bandwidth(udp_minus_filename_u))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -J --logfile " + tcp_filename_d)
        result_tcp_d.append(read_tcp_bandwidth(tcp_filename_d))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp_d[i])) + " -l 1400 -J --logfile " + udp_filename_d)
        result_udp_d.append(read_udp_bandwidth(udp_filename_d))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp_d[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_d)
        result_udp_plus_d.append(read_udp_bandwidth(udp_plus_filename_d))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp_d[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_d)
        result_udp_minus_d.append(read_udp_bandwidth(udp_minus_filename_d))


        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -J --logfile " + tcp_filename_u5)
        result_tcp5_u.append(read_tcp_bandwidth(tcp_filename_u5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp5_u[i])) + " -l 1400 -J --logfile " + udp_filename_u5)
        result_udp5_u.append(read_udp_bandwidth(udp_filename_u5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp5_u[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_u5)
        result_udp_plus5_u.append(read_udp_bandwidth(udp_plus_filename_u5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp5_u[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_u5)
        result_udp_minus5_u.append(read_udp_bandwidth(udp_minus_filename_u5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -J --logfile " + tcp_filename_d5)
        result_tcp_5_d.append(read_tcp_bandwidth(tcp_filename_d5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp_5_d[i])) + " -l 1400 -J --logfile " + udp_filename_d5)
        result_udp_5_d.append(read_udp_bandwidth(udp_filename_d5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp_5_d[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_d5)
        result_udp_plus_5_d.append(read_udp_bandwidth(udp_plus_filename_d5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp_5_d[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_d5)
        result_udp_minus_5_d.append(read_udp_bandwidth(udp_minus_filename_d5))


        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -J --logfile " + tcp_filename_u15)
        result_tcp15_u.append(read_tcp_bandwidth(tcp_filename_u15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp15_u[i])) + " -l 1400 -J --logfile " + udp_filename_u15)
        result_udp15_u.append(read_udp_bandwidth(udp_filename_u15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp15_u[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_u15)
        result_udp_plus15_u.append(read_udp_bandwidth(udp_plus_filename_u15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp15_u[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_u15)
        result_udp_minus15_u.append(read_udp_bandwidth(udp_minus_filename_u15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -J --logfile " + tcp_filename_d15)
        result_tcp_15_d.append(read_tcp_bandwidth(tcp_filename_d15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp_15_d[i])) + " -l 1400 -J --logfile " + udp_filename_d15)
        result_udp_15_d.append(read_udp_bandwidth(udp_filename_d15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp_15_d[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_d15)
        result_udp_plus_15_d.append(read_udp_bandwidth(udp_plus_filename_d15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp_15_d[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_d15)
        result_udp_minus_15_d.append(read_udp_bandwidth(udp_minus_filename_d15))

    write_result("tcpu.csv", result_tcp_u)
    write_result("udpu.csv", result_udp_u)
    write_result("udpPlusu.csv", result_udp_plus_u)
    write_result("udpMinusu.csv", result_udp_minus_u)
    write_result("tcpd.csv", result_tcp_d)
    write_result("udpd.csv", result_udp_d)
    write_result("udpPlusd.csv", result_udp_plus_d)
    write_result("udpMinusd.csv", result_udp_minus_d)

    write_result("tcp5u.csv", result_tcp5_u)
    write_result("udp5u.csv", result_udp5_u)
    write_result("udpPlus5u.csv", result_udp_plus5_u)
    write_result("udpMinus5u.csv", result_udp_minus5_u)
    write_result("tcp5d.csv", result_tcp_5_d)
    write_result("udp5d.csv", result_udp_5_d)
    write_result("udpPlus5d.csv", result_udp_plus_5_d)
    write_result("udpMinus5d.csv", result_udp_minus_5_d)

    write_result("tcp15u.csv", result_tcp15_u)
    write_result("udp15u.csv", result_udp15_u)
    write_result("udpPlus15u.csv", result_udp_plus15_u)
    write_result("udpMinus15u.csv", result_udp_minus15_u)
    write_result("tcp15d.csv", result_tcp_15_d)
    write_result("udp15d.csv", result_udp_15_d)
    write_result("udpPlus15d.csv", result_udp_plus_15_d)
    write_result("udpMinus15d.csv", result_udp_minus_15_d)


if __name__ == "__main__":
    interface = sys.argv[1]
    pcap_process = multiprocessing.Process(target=write_pcap, args=("iperf_log.pcap", interface))
    pcap_process.start()
    main()
    pcap_process.terminate()
