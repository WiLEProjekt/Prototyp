import csv, os, sys, getopt, ftplib, zipfile, json, time
import urllib.request

def readCSVFile(folder, filename):
    path = os.getcwd()+"/"+folder
    file = open(os.path.join(path,filename), 'r', encoding='utf-8')
    reader = csv.reader(file, skipinitialspace=True, delimiter=';')
    #next(reader) #skip header
    data = []
    for row in reader:
        data.append(row)
    file.close()
    return data

def writeJSON(filename, data):
    outfile = open(filename, 'w', encoding='utf-8')
    json.dump(data, outfile, ensure_ascii=False)
    outfile.close

def readStationList(fname):
    file = open(fname, 'r')
    data = file.readlines()
    indicator=data[2]
    indices = findIndices(indicator)
    del data[0:3] #delete header lines
    stations=[]
    for line in data:
        parts=[line[indices[i]:indices[i+1]].strip() for i in range(len(indices)-1)]
        stations.append(parts)
    return(stations)

def findIndices(string):
    indices=[0]
    for i,c in enumerate(string):
        if c==' ':
            indices.append(i)
    del indices[-1] #at the end theres a double ' '
    return(indices)

if __name__ == "__main__":
    ########################
    # parameter parsing
    ########################
    directory=''
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hd:", ["directory"])
    except getopt.GetoptError:
        print("Usage: python3 WeatherGrabber.py -d <directory>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -d <directory>")
            sys.exit()
        elif opt in ("-d", "--directory"):
            directory = arg

    ########################
    #Grab File with all current stations and save it
    ########################
    uf = urllib.request.urlopen("https://www.dwd.de/DE/leistungen/klimadatendeutschland/statliste/statlex_rich.txt?view=nasPublication&nn=16102")
    stationData = uf.read()
    open('stations.txt', 'w').write(stationData.decode("iso-8859-1"))
    stations = readStationList("stations.txt")

    ########################
    #filter only active stations.
    ########################
    activeStations = []
    for l in stations:
        enddateString = l[10]
        enddateYear = enddateString[-4:]
        if enddateYear == "2018" or enddateYear == "2019":
            activeStations.append(l)




    places = os.listdir(directory)
    for place in places:
        measurements = os.listdir(directory+'/'+place)
        for measurement in measurements:
            file = open(directory+'/'+place+'/'+measurement+'/coordinates.txt')
            timestamp = measurement.replace('-','')
            (date, hour, mins, sec, msec, city, placex, region) = timestamp.split('_')

            #######################
            # grab langitude and longitude
            #######################
            coordinates=file.readlines()
            langitude = coordinates[0]
            longitude = coordinates[1]
            #print(langitude, longitude)

            ########################
            # search nearest station to given coordinates
            ########################
            closest = 100000.0
            closestStation = []
            for l in activeStations:
                if l[2] == "MI" or l[2] == "MN": #assure that 10min data is available from that station
                    diff = abs(float(l[4])-float(langitude))+abs(float(l[5])-float(longitude)) #Manhattan Distance, maybe euclidian would be better
                    if diff < closest:
                        closest = diff
                        closestStation = l
            #print("Closest Station with 10min Values: {}".format(closestStation))
            stationID=""
            if((5 - len(closestStation[1])) > 0):
                stationID="0"*(5-len(closestStation[1]))+closestStation[1]
            else:
                stationID=closestStation[1]

            ########################
            # download precipitation (niederschlag) file from ftp
            ########################
            path = 'pub/CDC/observations_germany/climate/10_minutes/precipitation/now/'
            filename = '10minutenwerte_nieder_'+stationID+'_now.zip'
            ftp = ftplib.FTP("ftp-cdc.dwd.de")
            ftp.login()
            ftp.cwd(path)
            ftp.retrbinary("RETR "+filename, open(filename, 'wb').write)
            ftp.quit

            ########################
            # download air-temperature file from ftp
            ########################
            path = 'pub/CDC/observations_germany/climate/10_minutes/air_temperature/now/'
            filename = '10minutenwerte_TU_' + stationID + '_now.zip'
            ftp = ftplib.FTP("ftp-cdc.dwd.de")
            ftp.login()
            ftp.cwd(path)
            ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
            ftp.quit

            ########################
            # download wind file from ftp
            ########################
            path = 'pub/CDC/observations_germany/climate/10_minutes/wind/now/'
            filename = '10minutenwerte_wind_' + stationID + '_now.zip'
            ftp = ftplib.FTP("ftp-cdc.dwd.de")
            ftp.login()
            ftp.cwd(path)
            ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
            ftp.quit

            ########################
            # download solar (solarestrahlung file from ftp
            ########################
            path = 'pub/CDC/observations_germany/climate/10_minutes/solar/now/'
            filename = '10minutenwerte_SOLAR_' + stationID + '_now.zip'
            ftp = ftplib.FTP("ftp-cdc.dwd.de")
            ftp.login()
            ftp.cwd(path)
            ftp.retrbinary("RETR " + filename, open(filename, 'wb').write)
            ftp.quit

            ########################
            # create folder for the raw data files
            ########################
            try:
                os.mkdir("tempRawData")
            except OSError:
                pass

            ########################
            # extract zip archives, delete archives
            ########################
            zipf = zipfile.ZipFile('10minutenwerte_nieder_'+stationID+'_now.zip', 'r')
            zipf.extractall("tempRawData")
            os.remove('10minutenwerte_nieder_' + stationID + '_now.zip')
            zipf = zipfile.ZipFile('10minutenwerte_TU_' + stationID + '_now.zip', 'r')
            zipf.extractall("tempRawData")
            os.remove('10minutenwerte_TU_' + stationID + '_now.zip')
            zipf = zipfile.ZipFile('10minutenwerte_wind_' + stationID + '_now.zip', 'r')
            zipf.extractall("tempRawData")
            os.remove('10minutenwerte_wind_' + stationID + '_now.zip')
            zipf = zipfile.ZipFile('10minutenwerte_SOLAR_' + stationID + '_now.zip', 'r')
            zipf.extractall("tempRawData")
            os.remove('10minutenwerte_SOLAR_' + stationID + '_now.zip')
            zipf.close()

            ########################
            # read data from extracted files, delete old files and write new one
            ########################
            precipitation = []
            temperature = []
            wind = []
            solar = []
            #rr = precipitation (Niederschlag)
            #tu = air temperature
            #ff = wind
            #sd = solar
            for file in os.listdir("tempRawData"):
                if file.endswith(".txt"):
                    (produkt, interval, now, category, startdate, enddate, staID) = file.split("_")
                    if category == "rr":
                        precipitation = readCSVFile("tempRawData", file)
                    elif category == "tu":
                        temperature = readCSVFile("tempRawData", file)
                    elif category == "ff":
                        wind = readCSVFile("tempRawData", file)
                    elif category == "sd":
                        solar = readCSVFile("tempRawData", file)
                    else:
                        print("Invalid RawData file.")
                    os.remove('tempRawData/'+file)

            ########################
            # find the index with the closest timestamp to our measurement
            ########################
            timemeasurement=date+hour+mins
            i=0;
            for l in precipitation:
                if i == 0:
                    i += 1
                else:
                    diff = abs(int(timemeasurement)-int(l[1]))
                    if diff < 10:
                        print(l[1], timemeasurement)
                        break
                    else:
                        i += 1

            ########################
            # Add Data to Dictionary and write it as JSON-File
            ########################
            dataDict = {}
            dataDict['Niederschlagsdauer'] = precipitation[i][3]+' min'
            dataDict['Niederschlagshoehe'] = precipitation[i][4]+' mm'
            dataDict['Luftdruck'] = temperature[i][3]+' hPa'
            dataDict['Temperatur2m'] = temperature[i][4]+' °C'
            dataDict['Temperatur5cm'] = temperature[i][5]+' °C'
            dataDict['RelativeFeuchte'] = temperature[i][6]+' %'
            dataDict['Taupunkttemperatur'] = temperature[i][7]+' °C'
            dataDict['Windgeschwindigkeit'] = wind[i][3]+' m/s'
            dataDict['Windrichtung'] = wind[i][4]+' Grad'
            dataDict['SolareStrahlung'] = solar[i][3]+' J/cm²'
            dataDict['Globalstrahlung'] = solar[i][4]+' J/cm²'
            dataDict['Sonnenscheindauer'] = solar[i][5]+' h'
            dataDict['AtmosphaerischeGegenstrahlung'] = solar[i][6]+' J/cm²'
            writeJSON(directory+'/'+place+'/'+measurement+'/data.json', dataDict)
            time.sleep(1)