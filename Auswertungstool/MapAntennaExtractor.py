# Split the LTE_Telekom_Sender.csv into the used sender of the connection and the unsused.
# They are going to be coloured differently.

import sys, getopt, os, math, datetime, time

# convert the format of the gps receiver to decimaldegree
def convertToDecimal(value):
    v = value / 100
    degree = (str(v)).split('.')[0]
    minute = float(value % 100)
    dec_degree = float(minute) / 60 + float(degree)
    return dec_degree
 
    
if __name__ == "__main__":
    argv = sys.argv[1:]

    mapAttributes_path = ""
    lteSender_path = ""

    try:
        opts, args = getopt.getopt(argv, "hs:l:", ["mapAttributes", "lteAntennas"])
    except getopt.GetoptError:
        print("Usage: python3 MapAntennaExtractor.py -s <mapAttributes_path> -l <lteAntennas_filepath>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -s <mapAttributes_path> -l <lteAntennas_filepath>")
            sys.exit()
        elif opt in ("-s", "--mapAttributes"):
            mapAttributes_path = "./" + arg
        elif opt in ("-l", "--lteAntennas"):
            lteSender_path = "./" + arg
        
    if mapAttributes_path == "" and lteSender_path== "":
        print("Wrong parameters: " + gps_path + " " + signalstrength_path + "\n")
        print("Parameters: -s <mapAttributes_path> -l <lteAntennas_filepath>")
        sys.exit()


    #concat output filename
    parts = mapAttributes_path.split('_')
    origin_result1 = parts[0] + "_" + parts[1] + "_" + parts[2] + "_" + parts[3] + "_Active_CellTower_Locations.csv"
    origin_result2 = parts[0] + "_" + parts[1] + "_" + parts[2] + "_" + parts[3] + "_Non_Active_CellTower_Locations.csv"

    active_sender_file = open(origin_result1, "w") 
    active_sender_file.write("Latitude, Longtitude, id, Cellid \n")

    activeSenderLocations = []
    celllist = [] # mark already inserted cellids
    mapAttributes_file=open(mapAttributes_path,'r')
    for line in mapAttributes_file:
            cellid = line.split(',')[2]
            lte_sender_file = open(lteSender_path, 'r')
            for l in lte_sender_file:
                lat,lon,z,ident,cell = l.split(',')
                if cell.split('\n')[0] == cellid and not cellid in celllist:
                    activeSenderLocations.append(ident)
                    celllist.append(cellid)
                    active_sender_file.write(lat + "," + lon + "," + ident + "," + cell)
                    active_sender_file.flush()
            lte_sender_file.close()
    mapAttributes_file.close()
    active_sender_file.close()
      
    non_active_sender_file = open(origin_result2, "w")
    non_active_sender_file.write("Latitude, Longtitude, id, Cellid \n")    
    lte_sender_file = open(lteSender_path, 'r')
    for line in lte_sender_file:
        lat,lon,z,ident,cell = line.split(',')
        if not ident in activeSenderLocations:
            non_active_sender_file.write(lat + "," + lon + "," + ident + "," + cell)
    lte_sender_file.close()
    non_active_sender_file.close()

sys.exit(0)
