import os


def deleteZeros(filename):
    infile = open(filename, "r")
    lines = infile.readlines()
    infile.close()
    outFile = open(filename + "_new", "w")
    print("new file: " + filename + "_new")
    for line in lines:
        if line != "0\n" and line != "0":
            outFile.write(line)
    outFile.close()


def main():
    for root, subdirs, files in os.walk('.'):
        for file in files:
            if file == "downloadDelays.csv" or file == "uploadDelays.csv":
                print(root + "/" + file)
                deleteZeros(root + "/" + file)


if __name__ == "__main__":
    main()