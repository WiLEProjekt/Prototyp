import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests
from datetime import datetime
from subprocess import call
from threading import Thread
import threading

# Class to measure signal strength parameters with huaweiE3372 stick
import xmltodict


class HuaweiE3372(object):
  BASE_URL = 'http://{host}'
  COOKIE_URL = '/html/index.html'
  XML_APIS = [
    '/api/device/information',
    '/api/device/signal',
    '/api/monitoring/status',
    '/api/monitoring/traffic-statistics',
    '/api/dialup/connection',
    '/api/global/module-switch',
    '/api/net/current-plmn',
    '/api/net/net-mode',
  ]
  session = None

  def __init__(self,host='192.168.8.1'):
    self.host = host
    self.base_url = self.BASE_URL.format(host=host)
    self.session = requests.Session()
    # get a session cookie by requesting the COOKIE_URL
    r = self.session.get(self.base_url + self.COOKIE_URL)

  def get(self,path):
    return xmltodict.parse(self.session.get(self.base_url + path).text).get('response',None)




##### GLOBAL VARIABLES ############

packetctr = 0
timeout=False
measurementID = "" # public because used in different functions

def signalstrength(killevent, measurementId):
    e3372 = HuaweiE3372()
    file = open(measurementId + "_signal.csv", "w")
    while killevent.is_set():
        file.write(str(time.time()))
        for path in e3372.XML_APIS:
            for key, value in e3372.get(path).items():
                # print(key,value)
                if (key == u'FullName'):
                    file.write(";" + str(value))
                if (key == u'workmode'):
                    file.write(";" + str(value))
                if (key == u'rsrq'):
                    file.write(";" + str(value))
                if (key == u'rssi'):
                    file.write(";" + str(value))
                if (key == u'sinr'):
                    file.write(";" + str(value))
                if (key == u'rsrp'):
                    file.write(";" + str(value))
        file.write("\n")
        time.sleep(0.5)
    file.close()

def logGPS(filepath, killevent):
    with open(filepath + "_gps.txt", 'w') as output:
        call(["sudo", "python2", "gpsreader.py", "dump"], stdout=output)


def logNTP_Status(filepath, killevent):
    with open(filepath + "_ntp_status.txt", 'w') as output:
        while killevent.is_set():
            output.write("Timestamp: " + str(time.time()) + "\n")
            output.flush()
            # similar to sudo ntpq -np >> filepath
            call(["sudo", "ntpq", "-np"], stdout=output)
            time.sleep(1)

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
    firstPacket = True
    previousTs = 0.0
    while killevent.is_set():
        try:
            data, addr = udpsock.recvfrom(2048)            
            packetctr += 1 
            if (packetctr % 999) == 0: # each 1000 packets a timestamp is added to list
                currentTs = time.time() # current time in sec
                if (firstPacket):
                    previousTs = currentTs
                    firstPacket=False
                else:
                    # 1000 packets with 1000*8 bits goodput divided by passed time (which is in sec)
                    currentBandwidth = (1000 * 1000*8) / ((currentTs- previousTs)) / 1024
                    print("Current Bandwidth: " + str(currentBandwidth) + " Kbit/s")
                    previousTs = currentTs
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

    # setup socket and start message for server
    udpsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udpsock.settimeout(2)
    Message = str(packetsize)+"_"+str(bandwidth)
    currenttime = formatTimestamp()
    measurementID = currenttime + "_Train_MS_OS_Client"
    os.mkdir(measurementID)
    dirpath = measurementID + "/" + measurementID

    # pcap must be logged by a process
    pcap_process = multiprocessing.Process(target=write_pcap, args=(dirpath, interface))
    pcap_process.start()

    # synchronize other functionalities
    sendstart = threading.Event()
    killevent = threading.Event()
    killevent.set()
    threads = []
    recthread = Thread(target=receiveMSGS, args=(udpsock, sendstart, killevent))
    sendthread = Thread(target=sendStart, args=(udpsock, Message, destIP, destPort, sendstart, killevent))
    ntpthread = Thread(target=logNTP_Status, args=(dirpath, killevent))
    gpsthread = Thread(target=logGPS, args=(dirpath,killevent))
    signalthread = Thread(target=signalstrength, args=(killevent, dirpath))
    threads.append(recthread)
    threads.append(sendthread)
    threads.append(ntpthread)
    threads.append(gpsthread)
    threads.append(signalthread)
    signalthread.start()
    recthread.start()
    sendthread.start()
    ntpthread.start()
    gpsthread.start()

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
