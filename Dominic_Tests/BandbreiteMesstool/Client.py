import sys, getopt, os, json
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

def main(argv):
    ################################
    # Terminal argument parser
    ################################
    region = ''
    name = ''
    try:
        opts, args = getopt.getopt(argv, "hr:n:", ["region=", "name"])
    except getopt.GetoptError:
        print("Usage: python3 Client.py -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -r <region, [urban|suburban|rural]> -n <regionname e.g Osnabrueck>")
            sys.exit()
        elif opt in ("-r", "--region"):
            region = arg
        elif opt in ("-n", "--name"):
            name = arg

    ################################
    # Create the measurementID
    ################################
    currenttime = str(datetime.now())
    measurementID = region + "_" + name + "_" + currenttime #TODO add signal strength

    ################################
    # TCP Bandwidth Measurement
    ################################
    threads = []
    t1 = Thread(target=uploadBandwidth, args=())
    t2 = Thread(target=downloadBandwidth, args=())
    threads.append(t1)
    threads.append(t2)
    t1.start()
    t2.start()
    for thread in threads:  # Wait till all threads are finished
        thread.join()

    ################################
    # Fetch Bandwidth results into variable
    ################################
    uploadspeed = readBandwidth("upload.json")
    downloadspeed = readBandwidth("download.json")
    print("Upload: {} bit/sec".format(uploadspeed))
    print("Download: {} bit/sec".format(downloadspeed))


if __name__ == "__main__":
    main(sys.argv[1:])
