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
    tcp_filename_down = "tcp.json"
    udp_filename_down = "udp.json"
    udp_plus_filename_down = "udppluss.json"
    udp_minus_filename_down = "udpminus.json"

    tcp_filename_up = "tcpup.json"
    udp_filename_up = "udpup.json"
    udp_plus_filename_up = "udpplussup.json"
    udp_minus_filename_up = "udpminusup.json"

    tcp_filename_down5 = "tcp5.json"
    udp_filename_down5 = "udp5.json"
    udp_plus_filename_down5 = "udppluss5.json"
    udp_minus_filename_down5 = "udpminus5.json"

    tcp_filename_up5 = "tcpup5.json"
    udp_filename_up5 = "udpup5.json"
    udp_plus_filename_up5 = "udpplussup5.json"
    udp_minus_filename_up5 = "udpminusup5.json"

    tcp_filename_down15 = "tcp15.json"
    udp_filename_down15 = "udp15.json"
    udp_plus_filename_down15 = "udppluss15.json"
    udp_minus_filename_down15 = "udpminus15.json"

    tcp_filename_up15 = "tcpup15.json"
    udp_filename_up15 = "udpup15.json"
    udp_plus_filename_up15 = "udpplussup15.json"
    udp_minus_filename_up15 = "udpminusup15.json"

    result_tcp = []
    result_udp = []
    result_udp_plus = []
    result_udp_minus = []

    result_tcp_up = []
    result_udp_up = []
    result_udp_plus_up = []
    result_udp_minus_up = []

    result_tcp5 = []
    result_udp5 = []
    result_udp_plus5 = []
    result_udp_minus5 = []

    result_tcp_up5 = []
    result_udp_up5 = []
    result_udp_plus_up5 = []
    result_udp_minus_up5 = []


    result_tcp15 = []
    result_udp15 = []
    result_udp_plus15 = []
    result_udp_minus15 = []

    result_tcp_up15 = []
    result_udp_up15 = []
    result_udp_plus_up15 = []
    result_udp_minus_up15 = []

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

        try:
            os.remove(tcp_filename_down5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_down5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_down5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_down5)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_up5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_up5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_up5)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_up5)
        except FileNotFoundError:
            pass

        try:
            os.remove(tcp_filename_down15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_down15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_down15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_down15)
        except FileNotFoundError:
            pass
        try:
            os.remove(tcp_filename_up15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_filename_up15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_plus_filename_up15)
        except FileNotFoundError:
            pass
        try:
            os.remove(udp_minus_filename_up15)
        except FileNotFoundError:
            pass

        os.system("iperf3 -c 131.173.33.228 -p 50000 -J --logfile " + tcp_filename_down)
        result_tcp.append(read_tcp_bandwidth(tcp_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_down)
        result_udp.append(read_udp_bandwidth(udp_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_down)
        result_udp_plus.append(read_udp_bandwidth(udp_plus_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + str(int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_down)
        result_udp_minus.append(read_udp_bandwidth(udp_minus_filename_down))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -J --logfile " + tcp_filename_up)
        result_tcp_up.append(read_tcp_bandwidth(tcp_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_up)
        result_udp_up.append(read_udp_bandwidth(udp_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_up)
        result_udp_plus_up.append(read_udp_bandwidth(udp_plus_filename_up))

        os.system("iperf3 -c 131.173.33.228 -p 50000 -R -u -b " + str(
            int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_up)
        result_udp_minus_up.append(read_udp_bandwidth(udp_minus_filename_up))


        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -J --logfile " + tcp_filename_down5)
        result_tcp5.append(read_tcp_bandwidth(tcp_filename_down5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_down5)
        result_udp5.append(read_udp_bandwidth(udp_filename_down5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_down5)
        result_udp_plus5.append(read_udp_bandwidth(udp_plus_filename_down5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -u -b " + str(int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_down5)
        result_udp_minus5.append(read_udp_bandwidth(udp_minus_filename_down5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -J --logfile " + tcp_filename_up5)
        result_tcp_up5.append(read_tcp_bandwidth(tcp_filename_up5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_up5)
        result_udp_up5.append(read_udp_bandwidth(udp_filename_up5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_up5)
        result_udp_plus_up5.append(read_udp_bandwidth(udp_plus_filename_up5))

        os.system("iperf3 -c 131.173.33.228 -t 5 -p 50000 -R -u -b " + str(
            int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_up5)
        result_udp_minus_up5.append(read_udp_bandwidth(udp_minus_filename_up5))


        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -J --logfile " + tcp_filename_down15)
        result_tcp15.append(read_tcp_bandwidth(tcp_filename_down15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_down15)
        result_udp15.append(read_udp_bandwidth(udp_filename_down15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_down15)
        result_udp_plus15.append(read_udp_bandwidth(udp_plus_filename_down15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -u -b " + str(int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_down15)
        result_udp_minus15.append(read_udp_bandwidth(udp_minus_filename_down15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -J --logfile " + tcp_filename_up15)
        result_tcp_up15.append(read_tcp_bandwidth(tcp_filename_up15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp[i])) + " -l 1400 -J --logfile " + udp_filename_up15)
        result_udp_up15.append(read_udp_bandwidth(udp_filename_up15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp[i] * 1.1)) + " -l 1400 -J --logfile " + udp_plus_filename_up15)
        result_udp_plus_up15.append(read_udp_bandwidth(udp_plus_filename_up15))

        os.system("iperf3 -c 131.173.33.228 -t 15 -p 50000 -R -u -b " + str(
            int(result_tcp[i] / 1.1)) + " -l 1400 -J --logfile " + udp_minus_filename_up15)
        result_udp_minus_up15.append(read_udp_bandwidth(udp_minus_filename_up15))

    write_result("tcp.csv", result_tcp)
    write_result("udp.csv", result_udp)
    write_result("udpPlus.csv", result_udp_plus)
    write_result("udpMinus.csv", result_udp_minus)
    write_result("tcpup.csv", result_tcp_up)
    write_result("udpup.csv", result_udp_up)
    write_result("udpPlusup.csv", result_udp_plus_up)
    write_result("udpMinusup.csv", result_udp_minus_up)

    write_result("tcp5.csv", result_tcp5)
    write_result("udp5.csv", result_udp5)
    write_result("udpPlus5.csv", result_udp_plus5)
    write_result("udpMinus5.csv", result_udp_minus5)
    write_result("tcpup5.csv", result_tcp_up5)
    write_result("udpup5.csv", result_udp_up5)
    write_result("udpPlusup5.csv", result_udp_plus_up5)
    write_result("udpMinusup5.csv", result_udp_minus_up5)

    write_result("tcp15.csv", result_tcp15)
    write_result("udp15.csv", result_udp15)
    write_result("udpPlus15.csv", result_udp_plus15)
    write_result("udpMinus15.csv", result_udp_minus15)
    write_result("tcpup15.csv", result_tcp_up15)
    write_result("udpup15.csv", result_udp_up15)
    write_result("udpPlusup15.csv", result_udp_plus_up15)
    write_result("udpMinusup15.csv", result_udp_minus_up15)


if __name__ == "__main__":
    interface = sys.argv[1]
    pcap_process = multiprocessing.Process(target=write_pcap, args=("iperf_log.pcap", interface))
    pcap_process.start()
    main()
    pcap_process.terminate()
