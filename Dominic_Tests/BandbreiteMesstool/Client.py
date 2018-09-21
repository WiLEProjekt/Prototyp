import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal
from datetime import datetime
from threading import Thread

def uploadBandwidth():
    try:
        os.remove("upload.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50000 -J --logfile upload.json") #TODO testen ob packetsize limitiert werden muss

def downloadBandwidth():
    try:
        os.remove("download.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50001 -R -J --logfile download.json")  # TODO testen ob packetsize limitiert werden muss

def readBandwidth(filename):
    file = open(filename, "r")
    json_file = json.load(file)
    bandwidth = json_file['end']['sum_received']['bits_per_second']
    return(bandwidth)

def CBRupload(speed, a):
    os.system("iperf3 -c 131.173.33.228 -p 50000 -u -b " + speed + " -l 1450")

def CBRdownload(speed, a):
    os.system("iperf3 -c 131.173.33.228 -p 50001 -u -b " + speed + " -l 1450 -R")

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
    pcapfilename = "client_"+measurementID

    ################################
    # TCP Bandwidth Measurement
    ################################
    threads1 = []
    t1 = Thread(target=uploadBandwidth, args=())
    t2 = Thread(target=downloadBandwidth, args=())
    threads1.append(t1)
    threads1.append(t2)
    t1.start()
    t2.start()
    for thread in threads1:  # Wait till all threads are finished
        thread.join()

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
    cbr = int(min(uploadspeed, downloadspeed)/2)
    cbrstring = str(cbr)
    destIP = "131.173.33.228"
    destPort = 50002
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    tcpsock.send(measurementID.encode())

    time.sleep(1)
    threads2 = []
    t3 = Thread(target=CBRupload, args=(cbrstring, 1))
    t4 = Thread(target=CBRdownload, args=(cbrstring, 1))
    pcap_process = multiprocessing.Process(target=write_pcap, args=(pcapfilename, interface ))
    pcap_process.start()
    threads2.append(t3)
    threads2.append(t4)
    t3.start()
    t4.start()
    for thread2 in threads2:  # Wait till all threads are finished
        thread2.join()
    pcap_process.terminate()
    tcpsock.close()

if __name__ == "__main__":
    main(sys.argv[1:])
