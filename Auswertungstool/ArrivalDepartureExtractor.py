# Extract unix timestamps at arrival and departure 
# at / from stations and significant areas

import sys, getopt, os, math, datetime, time

def squaredDistanceBetweenLocation(fromLat, fromLon, toLat, toLon):
    try:
        deltaLat = abs(float(fromLat) - float(toLat))
        deltaLon = abs(float(fromLon) - float(toLon))
        return float(deltaLat * deltaLat + deltaLon * deltaLon)
    except ValueError:
        return sys.float_info.max
    
if __name__ == "__main__":
    argv = sys.argv[1:]

    mapAttr_path = "2018-12-19_09_14_08_MapAttributes.csv"
    arrDep_path = "An_Ab_Mue_Osna_Bahnhoefe.csv"

    try:
        opts, args = getopt.getopt(argv, "hm:b:", ["mapAttributes", "bahnhoefe"])
    except getopt.GetoptError:
        print("Usage: python3 ArrivalDepartureExtractor.py -m <MapAttributes_filepath> -b <an_ab_bahnhoefe_filepath>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -m <MapAttributes_filepath> -s <an_ab_bahnhoefe_filepath>")
            sys.exit()
        elif opt in ("-m", "--mapAttributes"):
            mapAttr_path = "./" + arg
        elif opt in ("-b", "--bahnhoefe"):
            arrDep_path = "./" + arg

    #concat output filename
    parts = mapAttr_path.split('_')
    origin = parts[0] + "_" + parts[1] + "_" + parts[2] + "_" + parts[3] + "_MapAttributes.csv"

    result_file = open("Timestamp_Stations_Tunnel", "w") 
    result_file.write("Timestamp_Unix\n")

    arrDep_file=open(arrDep_path,'r')
    for line in arrDep_file:
        fromlon, fromlat = line.split(',')
        if fromlon == "X" or fromlat == "Y":
            continue
        mapAttr_file = open(mapAttr_path, 'r')
        min_distance = sys.float_info.max
        min_ts = sys.float_info.max
        for l in mapAttr_file:
            ts, tech, cell, lat, lon, a,b,c,d = l.split(',')
            dist = squaredDistanceBetweenLocation(fromlat, fromlon, lat, lon)
            if dist < min_distance:
                min_distance = dist
                min_ts = ts
        result_file.write(str(min_ts) + "\n") 
        result_file.flush()
        mapAttr_file.close()               
    arrDep_file.close()
    result_file.close()

sys.exit(0)
