# Merge the signalstrength and the gps by the timestamp. 
# To each gps-coordinate the nearest timestamp of a signal within a time 
# interval of 10 seconds is looked up.  

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

    gps_path = ""
    signalstrength_path = ""
    try:
        opts, args = getopt.getopt(argv, "hg:s:", ["gps", "signalstrength"])
    except getopt.GetoptError:
        print("Usage: python3 CoordinateExtractor.py -g <gps_filepath> -s <signalstrength_filepath>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -g <gps_filepath> -s <signalstrength_filepath>")
            sys.exit()
        elif opt in ("-g", "--gps"):
            gps_path = "./" + arg
        elif opt in ("-s", "--signalstrength"):
            signalstrength_path = "./" + arg
        
    if gps_path == "" and signalstrength_path== "":
        print("Wrong parameters: " + gps_path + " " + signalstrength_path + "\n")
        print("Parameters: -g <gps_filepath> -s <signalstrength_filepath>")
        sys.exit()

    #concat output filename
    parts = gps_path.split('/')[1].split('_')
    origin = parts[0] + "_" + parts[1] + "_" + parts[2] + "_" + parts[3] + "_MapAttributes.txt"

    result_file = open(origin, "w") 
    result_file.write("Timestamp_GPS, Cellid, Latitude, Longtitude, GPS_activated, NumberSatellites ,Signalstrength, Diff_TS_GPS_SS \n")
    gps_file=open(gps_path,'r')
    print(gps_file.name)
    ts_dmy = ""
    for line in gps_file:
        signalstrength = 0
        items =line.split(',')        
        if items[0] =="GPRMC": # only GPRMC contains day, month and year
            ts_dmy = items[9] # DayMonthYear
        if items[0] == "GPGGA" and ts_dmy != "": # GPGGA contains timestamp and coordinates
            ts_hms, ms =  items[1].split('.') # HourMinuteSecond . ms        
            lat_rad = convertToDecimal(float(items[2])) 
            lon_rad = convertToDecimal(float(items[4]))
            ts = datetime.datetime.strptime((ts_dmy + " " + ts_hms), '%d%m%y %H%M%S')
            ts_unix = time.mktime(ts.timetuple()) + (float(ms) / 1000) + 3600 # plus 3600 (= 60 sec * 60 min) because different time offset 
            numberSat = items[7]
            gps_activated = items[6]
            # find nearest signalstrength to textfile, within 10 seconds timediff
            signalstrength_file = open(signalstrength_path,'r')
            min_ts_diff = 10.0 # in seconds
            cellid = 0
            for line in signalstrength_file:
                items = line.split(';') # timestamp , technology, cellid, dont know, signalstrength, dont know...
                if items[1] == "LTE":
                    ts_diff = abs(float(items[0]) - ts_unix)
                    if ts_diff < min_ts_diff:
                        min_ts_diff = ts_diff
                        try:
                            signalstrength = items[4].split("d")[0]
                        except ValueError:
                            signalstrength = 0                   
                        try:
                            cellid = int(items[2])
                        except ValueError:
                            cellid = 0                        
            result_file.write(str(ts_unix) + "," + str(cellid) + "," + str(lat_rad) + "," + str(lon_rad) + "," + str(gps_activated) + "," + str(numberSat) + "," + str(signalstrength) + "," + str(min_ts_diff) + "\n")
            result_file.flush()
               
    gps_file.close()
    result_file.close()


sys.exit(0)
