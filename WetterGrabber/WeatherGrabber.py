import csv, os, sys, getopt, ftplib, zipfile, json

def readCSVFile(folder, filename):
    path = os.getcwd()+"/"+folder
    file = open(os.path.join(path,filename), 'r', encoding='utf-8')
    reader = csv.reader(file, skipinitialspace=True, delimiter=';')
    #next(reader) #skip header
    data = []
    for row in reader:
        data.append(row)
    return data

def writeJSON(filename, data):
    outfile = open(filename, 'w', encoding='utf-8')
    json.dump(data, outfile, ensure_ascii=False)
    outfile.close

if __name__ == "__main__":
    ########################
    # parameter parsing
    ########################
    langitude = ''
    longitude = ''
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hx:y:", ["langitude", "longitude"])
    except getopt.GetoptError:
        print("Usage: python3 WeatherGrabber.py -x <langitude> -y <longitude>")
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print("Parameters: -x <langitude> -y <longitude>")
            sys.exit()
        elif opt in ("-x", "--langitude"):
            langitude = arg
        elif opt in ("-y", "--longitude"):
            longitude = arg

    # read all DWD Stations from file --> list(lists)
    stations = readCSVFile('DWDData','stationlistGermany.csv')

    ########################
    #filter only active stations.
    ########################
    activeStations = []
    for l in stations:
        enddateString = l[10]
        enddateYear = enddateString[-4:]
        if enddateYear == "2018" or enddateYear == "2019":
            activeStations.append(l)

    ########################
    # search nearest station to given coordinates
    ########################
    closest = 100000.0
    closestStation = []
    for l in activeStations:
        if l[2] == "MI" or l[2] == "MN": #assure that 10min data is available from that station
            diff = abs(float(l[4])-float(langitude))+abs(float(l[5])-float(longitude))
            if diff < closest:
                closest = diff
                closestStation = l
    print("Closest Station with 10min Values: {}".format(closestStation))
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
    # Add Data to Dictionary and write it as JSON-File
    ########################
    dataDict = {}
    dataDict['Niederschlagsdauer'] = precipitation[-1][3]+' min'
    dataDict['Niederschlagshoehe'] = precipitation[-1][4]+' mm'
    dataDict['Luftdruck'] = temperature[-1][3]+' hPa'
    dataDict['Temperatur2m'] = temperature[-1][4]+' °C'
    dataDict['Temperatur5cm'] = temperature[-1][5]+' °C'
    dataDict['RelativeFeuchte'] = temperature[-1][6]+' %'
    dataDict['Taupunkttemperatur'] = temperature[-1][7]+' °C'
    dataDict['Windgeschwindigkeit'] = wind[-1][3]+' m/s'
    dataDict['Windrichtung'] = wind[-1][4]+' Grad'
    dataDict['SolareStrahlung'] = solar[-1][3]+' J/cm²'
    dataDict['Globalstrahlung'] = solar[-1][4]+' J/cm²'
    dataDict['Sonnenscheindauer'] = solar[-1][5]+' h'
    dataDict['AtmosphaerischeGegenstrahlung'] = solar[-1][6]+' J/cm²'
    writeJSON('data.json', dataDict)