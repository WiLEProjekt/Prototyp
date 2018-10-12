import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal
from datetime import datetime
from threading import Thread

def uploadBandwidth():
    try:
        os.remove("upload.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50000 -t 60 -J --logfile upload.json") #TODO testen ob packetsize limitiert werden muss

def downloadBandwidth():
    try:
        os.remove("download.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50000 -t 60 -R -J --logfile download.json")  # TODO testen ob packetsize limitiert werden muss

def readBandwidth(filename):
    file = open(filename, "r")
    json_file = json.load(file)
    all_bandwidth = json_file['intervals']
    usefull_bandwidth = all_bandwidth[30:]
    tmp_bandwith = 0
    for bw in usefull_bandwidth:
        tmp_bandwith += bw['sum']['bits_per_second']

    return tmp_bandwith / len(usefull_bandwidth)

def CBRupload(speed):
    os.system("iperf3 -c 131.173.33.228 -p 50000 -u -t 60 -b " + speed + " -l 1450")

def CBRdownload(speed):
    os.system("iperf3 -c 131.173.33.228 -p 50000 -u -t 60 -b " + speed + " -l 1450 -R")

def signal_term_handler():
    sys.exit(0)

def write_pcap(filename, interface, filter):
    pcapy.findalldevs()
    max_bytes = 2048
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live(interface, max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open(filename + ".pcap")
    signal.signal(signal.SIGTERM, signal_term_handler)

    try:
        pc.setfilter(filter)
        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)
        packet_limit = -1 #infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()

def main(argv):
    ################################
    # Terminal argument parser
    ################################
    region = ''
    name = ''
    interface = ''
    try:
        opts, args = getopt.getopt(argv, "hr:n:i:", ["region", "name", "interface"])
    except getopt.GetoptError:
        print("Usage: python3 Client.py -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck> -i <networkinterface")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck> -i <networkinterface")
            sys.exit()
        elif opt in ("-r", "--region"):
            region = arg
        elif opt in ("-n", "--name"):
            name = arg
        elif opt in ("-i", "--interface"):
            interface = arg
    print(argv)

    ################################
    # Create the measurementID
    ################################
    currenttime = str(datetime.now())
    measurementID = region + "_" + name + "_" + currenttime #TODO add signal strength
    pcap_cbr_filename_slow = "client_cbr_slow_"+measurementID
    pcap_cbr_filename_fast = "client_cbr_fast_"+measurementID
    pcap_bw_filename_upload = "client_bw_upload_"+measurementID
    pcap_bw_filename_download = "client_bw_download_"+measurementID

    destIP = "131.173.33.228"
    destPort = 50001

    ################################
    # TCP Bandwidth Upload Measurement
    ################################
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idup = "server_bw_upload_"+measurementID
    tcpsock.send(idup.encode())
    pcap_bandwith_process = multiprocessing.Process(target=write_pcap, args=(pcap_bw_filename_upload, interface, 'tcp'))
    pcap_bandwith_process.start()
    time.sleep(1)
    uploadBandwidth()
    pcap_bandwith_process.terminate()
    tcpsock.close()

    ################################
    # TCP Bandwidth Download Measurement
    ################################
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    iddown = "server_bw_download_"+measurementID
    tcpsock.send(iddown.encode())
    pcap_bandwith_process = multiprocessing.Process(target=write_pcap, args=(pcap_bw_filename_download, interface, 'tcp'))
    pcap_bandwith_process.start()
    time.sleep(1)
    downloadBandwidth()
    pcap_bandwith_process.terminate()
    tcpsock.close()

    ################################
    # Fetch Bandwidth results into variable
    ################################
    uploadspeed = readBandwidth("upload.json")
    downloadspeed = readBandwidth("download.json")
    print("Upload: {} bit/sec".format(uploadspeed))
    print("Download: {} bit/sec".format(downloadspeed))

    ################################
    # Constant Bitrate
    ################################
    cbr_slow = int(min(uploadspeed, downloadspeed)/4)
    cbr_fast = int(min(uploadspeed, downloadspeed)*3/4)

    #slow cbr
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idcbrslow = "server_cbr_slow_"+measurementID
    tcpsock.send(idcbrslow.encode())
    pcap_process = multiprocessing.Process(target=write_pcap, args=(pcap_cbr_filename_slow, interface, 'udp'))
    pcap_process.start()
    time.sleep(1)
    CBRupload(str(cbr_slow))
    CBRdownload(str(cbr_slow))
    pcap_process.terminate()
    tcpsock.close()

    #fast cbr
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idcbrfast = "server_cbr_fast_"+measurementID
    tcpsock.send(idcbrfast.encode())
    pcap_process = multiprocessing.Process(target=write_pcap, args=(pcap_cbr_filename_fast, interface, 'udp'))
    pcap_process.start()
    time.sleep(1)
    CBRupload(str(cbr_fast))
    CBRdownload(str(cbr_fast))
    pcap_process.terminate()
    time.sleep(1)

    tcpsock.close()

if __name__ == "__main__":
    main(sys.argv[1:])
