import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests
from datetime import datetime
from threading import Thread
import threading

packetctr = 0
timeout=False
measurementID = "" # public because used in different functions
timestamp_list = [] # each entry contains a timestamp, a timestamp is added all 1000 packets


# write line-by-line the timestamps to a textfile
def writeTimestamp_list_to_file(filename):
    with open(filename,'w') as fd: 
        for timestamp in timestamp_list:
            fd.write("%s\n" % timestamp)

# signal handler for SIGINT pcap_thread
def signal_handler(signal, frame):
    sys.exit(0)

def write_pcap(filename, interface):
    pcapy.findalldevs()
    max_bytes = 1100
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live(interface, max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open(filename + ".pcap")
    signal.signal(signal.SIGINT, signal_handler)
    try:
        pc.setfilter('udp')

        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)

        packet_limit = -1  # infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()

def sendStart(udpsock, message, ip, port, sendstart, killevent):
    global timeout
    while killevent.is_set():
        sendstart.wait()
        if timeout:
            print("Send new heartbeats")
            for i in range(10): #send the start message
                udpsock.sendto(message.encode(), (ip, port))
            sendstart.clear()

def receiveMSGS(udpsock, sendstart, killevent):
    global timeout, packetctr
    while killevent.is_set():
        try:
            data, addr = udpsock.recvfrom(2048)            
            packetctr += 1 
            if (packetctr % 99) == 0: # each 1000 packets a timestamp is added to list
                currentTs = time.time() * 1000 # current time in ms
                if (len(timestamp_list) > 1):
                    previousTs = timestamp_list[-1]
                    # 100 packets with 1000*8 bits goodput divided by passed time (which is in ms) multiplied with 1000
                    currentBandwidth = (100 * 1000*8) / ((currentTs- previousTs))
                    print("Current Bandwidth: " + str(currentBandwidth) + " kbit/ms")
                timestamp_list.append(currentTs)
            #print("received package from {}".format(addr))
        except socket.timeout:
            print("connection lost")
            timeout=True
            sendstart.set()

def formatTimestamp():
    currenttime = str(datetime.now())
    currenttime = currenttime.replace(':', '_')
    currenttime = currenttime.replace('.', '_')
    currenttime = currenttime.replace(' ', '_')
    return currenttime

if __name__ == "__main__":
    argv = sys.argv[1:]
    destIP=""
    destPort=0
    packetsize=1000 #default 1000Byte
    bandwidth=1000 #default 1Mbit/s
    interface=""
    try:
        opts, args = getopt.getopt(argv, "hi:p:b:n:", ["destionation IP", "destination Port", "bandwidth", "interface"])
    except getopt.GetoptError:
        print("Usage: python3 Client.py -i <dest IP> -p <dest Port> -b <bandwidth [kbit/s> -n <interface>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -i <dest IP> -p <dest Port> -b <bandwidth [kbit/s> -n <interface>")
            sys.exit()
        elif opt in ("-i", "--ip"):
            destIP = arg
        elif opt in ("-p", "--port"):
            destPort = int(arg)
        elif opt in ("-b", "--bandwidth"):
            bandwidth = int(arg)
        elif opt in ("-n", "--interface"):
            interface = arg
    if destIP == "" or destPort is 0 or interface == "":
        print("No IP or no Port or no interface provided.")
        sys.exit(2)

    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.settimeout(2)
    Message = str(packetsize)+"_"+str(bandwidth)
    currenttime = formatTimestamp()
    measurementID = currenttime + "_Train_MS_OS_Client"
    pcap_process = multiprocessing.Process(target=write_pcap, args=(measurementID, interface))
    pcap_process.start()
    sendstart = threading.Event()
    killevent = threading.Event()
    killevent.set()
    threads = []
    recthread = Thread(target=receiveMSGS, args=(udpsock, sendstart, killevent))
    sendthread = Thread(target=sendStart, args=(udpsock, Message, destIP, destPort, sendstart, killevent))
    threads.append(recthread)
    threads.append(sendthread)
    recthread.start()
    sendthread.start()
    while len(threads)>0:
        try:
            for t in threads:
                t.join(1)
                if t.is_alive() is False:
                    threads.remove(t)
        except KeyboardInterrupt:
            print("keyboardinterrupt detected")
            sendstart.set()
            killevent.clear()
            time.sleep(3)
            filename = measurementID + "_Bandwidth_Timestamps.txt"
            writeTimestamp_list_to_file(filename)
            pcap_process.terminate()
