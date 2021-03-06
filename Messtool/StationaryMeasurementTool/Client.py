import sys, getopt, os, json, socket, time, multiprocessing, pcapy, signal, pprint, requests, xmltodict
from datetime import datetime
from threading import Thread
import threading

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

def signalstrength(kill, measurementId):
    e3372 = HuaweiE3372()

    file = open(measurementId + "/Signal.csv", "w")
    while not kill.is_set():
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

def uploadBandwidth(measurementId):
    try:
        os.remove(measurementId + "/upload.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50000 -t 60 -J --logfile " + measurementId + "/upload.json") #TODO testen ob packetsize limitiert werden muss

def downloadBandwidth(measurementId):
    try:
        os.remove(measurementId + "/download.json")
    except FileNotFoundError:
        pass
    os.system("iperf3 -c 131.173.33.228 -p 50000 -t 60 -R -J --logfile " + measurementId + "/download.json")  # TODO testen ob packetsize limitiert werden muss

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
    os.system("iperf3 -c 131.173.33.228 -p 50000 -u -t 60 -b " + speed + " -l 1000")

def CBRdownload(speed):
    os.system("iperf3 -c 131.173.33.228 -p 50000 -u -t 60 -b " + speed + " -l 1000 -R")

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
    langitude = ''
    longitude = ''
    try:
        opts, args = getopt.getopt(argv, "hr:n:i:x:y:", ["region", "name", "interface","langitude","longitude"])
    except getopt.GetoptError:
        print("Usage: python3 Client.py -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck> -i <networkinterface> -x <langitude> -y <longitude>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck> -i <networkinterface> -x <langitude> -y <longitude>")
            sys.exit()
        elif opt in ("-r", "--region"):
            region = arg
        elif opt in ("-n", "--name"):
            name = arg
        elif opt in ("-i", "--interface"):
            interface = arg
        elif opt in ("-x", "--langitude"):
            langitude = arg
        elif opt in ("-y", "--longitude"):
            longitude = arg

	
    #print(argv)

    ################################
    # Create the measurementID
    ################################
    currenttime = str(datetime.now())
    currenttime = currenttime.replace(':', '_')
    currenttime = currenttime.replace('.', '_')
    currenttime = currenttime.replace(' ', '_')
    measurementID = currenttime + "_" + name + "_" + region 


    if not os.path.exists(measurementID):
        os.makedirs(measurementID)

    file = open(measurementID + "/coordinates.txt", "w")
    file.write(langitude + "\n" + longitude)
    file.close()

    pcap_cbr_filename_slow = measurementID + "/client_cbr_slow"
    pcap_cbr_filename_fast = measurementID + "/client_cbr_fast"
    pcap_bw_filename_upload = measurementID + "/client_bw_upload"
    pcap_bw_filename_download = measurementID + "/client_bw_download"

    destIP = "131.173.33.228"
    destPort = 50001

    signalkill = threading.Event()
    signalthread = Thread(target=signalstrength, args=(signalkill, measurementID))
    signalthread.start()
    ################################
    # TCP Bandwidth Upload Measurement
    ################################
    #Send MEsaurmentID to the Server and signal the server that a new measurement begins
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idup = measurementID + "/server_bw_upload"
    tcpsock.send(idup.encode())

    pcap_bandwith_process = multiprocessing.Process(target=write_pcap, args=(pcap_bw_filename_upload, interface, 'tcp'))
    pcap_bandwith_process.start()
    time.sleep(1) #Make sure that pcap is started
    print("Measuring Upload-Goodput...\n")
    uploadBandwidth(measurementID)
    time.sleep(1)  #Make sure that measurement is finished and pcaps are logged
    pcap_bandwith_process.terminate()
    tcpsock.close()
    time.sleep(5)

    ################################
    # TCP Bandwidth Download Measurement
    ################################
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    iddown = measurementID + "/server_bw_download"
    tcpsock.send(iddown.encode())
    pcap_bandwith_process = multiprocessing.Process(target=write_pcap, args=(pcap_bw_filename_download, interface, 'tcp'))
    pcap_bandwith_process.start()
    time.sleep(1) #Make sure that pcap is started
    print("Measuring Download-Goodput...\n")
    downloadBandwidth(measurementID)
    time.sleep(1)  # Make sure that measurement is finished and pcaps are logged
    pcap_bandwith_process.terminate()
    tcpsock.close()
    time.sleep(5)

    ################################
    # Fetch Bandwidth results into variable
    ################################
    uploadspeed = readBandwidth(measurementID + "/upload.json")
    downloadspeed = readBandwidth(measurementID + "/download.json")

    print("Upload: {} bit/sec".format(uploadspeed))
    print("Download: {} bit/sec".format(downloadspeed))

    ################################
    # Constant Bitrate
    ################################
    cbr_slow = int(min(uploadspeed, downloadspeed)/4)
    cbr_fast = int(min(uploadspeed, downloadspeed)*3/4)

    ####slow cbr
    #Signal Server that slow_cbr is starting
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idcbrslow = measurementID + "/server_cbr_slow"
    tcpsock.send(idcbrslow.encode())
    #slow crb
    pcap_process = multiprocessing.Process(target=write_pcap, args=(pcap_cbr_filename_slow, interface, 'udp'))
    pcap_process.start()
    time.sleep(1) #Make sure that pcap is started
    print("Slow CBR Upload...\n")
    CBRupload(str(cbr_slow))
    print("Slow CBR Download...\n")
    CBRdownload(str(cbr_slow))
    time.sleep(1)  # Make sure that measurement is finished and pcaps are logged
    pcap_process.terminate()
    tcpsock.close()
    time.sleep(5)

    ####fast cbr
    #Signal Server that fast cbr is starting
    tcpsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # TCP
    tcpsock.connect((destIP, destPort))
    idcbrfast = measurementID + "/server_cbr_fast"
    tcpsock.send(idcbrfast.encode())
    #fast cbr
    pcap_process = multiprocessing.Process(target=write_pcap, args=(pcap_cbr_filename_fast, interface, 'udp'))
    pcap_process.start()
    time.sleep(1) #Make sure that pcap is started
    print("Fast CBR Upload...\n")
    CBRupload(str(cbr_fast))
    print("Fast CBR Download...\n")
    CBRdownload(str(cbr_fast))
    time.sleep(1)  # Make sure that measurement is finished and pcaps are logged
    pcap_process.terminate()
    print("Measurement finished.\n")

    tcpsock.close()

    signalkill.set()
    signalthread.join()

if __name__ == "__main__":
    main(sys.argv[1:])
