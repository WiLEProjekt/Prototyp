import csv, os, sys, getopt

def readTextList(filename):
    file = open(filename, 'r', encoding='iso-8859-1')
    for line in file:
        (name, id, kennung, statkennung, breite, laenge, hoehe, flussgebiet, bundesland, beginn, ende) = line.split(" ")
        print("{} {} {} {} {} {} {} {} {} {}".format(name, id, kennung, statkennung, breite, laenge, hoehe, flussgebiet,
                                                     bundesland, beginn, ende))

def readCSVFile(filename):
    path = os.getcwd()+"/DWDData"
    file = open(os.path.join(path,filename), 'r', encoding='utf-8')
    reader = csv.reader(file, delimiter=';')
    #next(reader) #skip header
    data = []
    for row in reader:
        data.append(row)
    return data

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

    print(langitude, longitude)

    # read all DWD Stations from file --> list(lists)
    stations = readCSVFile('stationlistGermany.csv')

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
        if l[2] == "MI" or l[2] == "MN":
            diff = abs(float(l[4])-float(langitude))+abs(float(l[5])-float(longitude))
            if diff < closest:
                closest = diff
                closestStation = l
    print(closestStation)
