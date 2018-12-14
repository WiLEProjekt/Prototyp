import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests
from datetime import datetime
from threading import Thread
import threading

sequencenumber = 0
clientPort = 5000  # default port
packetsize = 1 # will be overwritten by first packet received
bandwidth = 1 # will be overwritten by first packet received
clientIP=""

def signalhandler(signal, frame):
    sys.exit(0)


def write_pcap(filename, interface):
    pcapy.findalldevs()
    max_bytes = 1100
    promiscuous = False
    read_timeout = 100
    pc = pcapy.open_live(interface, max_bytes, promiscuous, read_timeout)
    dumper = pc.dump_open(filename + ".pcap")
    signal.signal(signal.SIGINT, signalhandler)

    try:
        pc.setfilter('udp')

        def recv_pkts(hdr, data):
            dumper.dump(hdr, data)

        packet_limit = -1  # infinite
        pc.loop(packet_limit, recv_pkts)
    except BaseException:
        pc.close()

def calcSequencenumber():
    global sequencenumber
    if (sequencenumber + 1) == sys.maxsize: #start with 0 if max int is reached
        sequencenumber = 0
    else:
        sequencenumber += 1

# address is used in PcapParser
def writeClientAdress_to_file(filename, addr, port):
    with open(filename,"w") as fd:
        fd.write(str(addr) + " ")
        fd.write(str(port))
    fd.close()

def sendCBR(udpsock, sendstart, killevent):
    global packetsize, bandwidth, clientIP, clientPort, sequencenumber
    while killevent.is_set():
            sendstart.wait()
            if not clientIP=="":
                #print("Start sending CBR")
                # compute cbr #
                bw_byte = ((bandwidth / 8) * 1000)  # original bandwidth is in kbit/s, now byte/s
                packetsNumber = bw_byte / packetsize # packets pro sec
                #print("packets/sec: {}".format(packetsNumber))
                sleepIntervall = 1 / packetsNumber
                #print("sleep intervall: {}".format(sleepIntervall))
                sequenceString = str(sequencenumber)
                Message = "0" * (packetsize - len(sequenceString)) + sequenceString
                #print("sending to {}:{}".format(clientIP, clientPort))
                udpsock.sendto(Message.encode(), (clientIP, clientPort))
                calcSequencenumber()
                time.sleep(sleepIntervall)


def receiveMSGS(udpsock,mdir, sendstart, killevent):
    global packetsize, clientIP, clientPort, bandwidth
    while killevent.is_set():
        try:
            data, addr = udpsock.recvfrom(2048)
            writeClientAdress_to_file(mdir + "_clientAdress.txt", addr[0], addr[1])
            #print(data.decode(), addr)
            clientIP = addr[0]
            clientPort = int(addr[1])
            datastr = data.decode()
            try:
                packetsize2, bandwidth2 = datastr.split('_')
                packetsize = int(packetsize2)
                bandwidth = int(bandwidth2)
                sendstart.set()
            except ValueError:#Packet with wrong payload received
                pass
        except socket.timeout:
            pass


def formatTimestamp():
    currenttime = str(datetime.now())
    currenttime = currenttime.replace(':', '_')
    currenttime = currenttime.replace('.', '_')
    currenttime = currenttime.replace(' ', '_')
    return currenttime

if __name__ == "__main__":
    argv = sys.argv[1:]
    ownIP = ""
    ownPort = 50000 # default
    interface=""
    try:
        opts, args = getopt.getopt(argv, "hi:p:n:", ["Server IP","Port","Interface"])
    except getopt.GetoptError:
        print("Usage: python3 Server.py -i <Server IP> -p <Port> -n <interface>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -i <own IP> -p <own Port> -n <interface>")
            sys.exit()
        elif opt in ("-i", "--ip"):
            ownIP = arg
        elif opt in ("-p", "--port"):
            ownPort = int(arg)
        elif opt in ("-n", "--interface"):
            interface = arg
    if ownIP == "" or interface == "":
        print("The IP address this server should listen on is missing or no Interface provided")
        sys.exit(2)

    # make unique folder for data
    currenttime = formatTimestamp()
    measurementID = currenttime + "_Train_MS_OS_Server"
    os.mkdir(measurementID)
    dirpath = measurementID + "/" + measurementID

    # setup server
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # UDP
    udpsock.bind((ownIP, ownPort))
    udpsock.settimeout(2)
    print("Server is listening on {}:{}".format(ownIP, ownPort))

    # pcap has to be started in own process
    pcap_process = multiprocessing.Process(target=write_pcap, args=(dirpath, interface))
    pcap_process.start()

    # synchro threads
    sendstart = threading.Event()
    killevent = threading.Event()
    killevent.set()
    threads=[]
    recthread = Thread(target=receiveMSGS, args=(udpsock, dirpath,sendstart, killevent))
    sendthread = Thread(target=sendCBR, args=(udpsock, sendstart, killevent))
    recthread.start()
    sendthread.start()
    threads.append(recthread)
    threads.append(sendthread)
    while len(threads)>0:
        try:
            for t in threads:
                t.join(1)
                if t.is_alive() is False:
                    threads.remove(t)
        except KeyboardInterrupt:
            print("Stopped measurement via KeyboardInterrupt. Finishing...")
            sendstart.set()
            killevent.clear()
            time.sleep(3)
            pcap_process.terminate()
