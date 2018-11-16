import sys
import os
import json


def readBandwidth(path, filename):
    result_path = path + "/Ergebnis"
    if not os.path.exists(result_path):
        os.makedirs(result_path)

    in_file = open(path + "/" + filename + ".json", "r")
    out_file = open(result_path + "/" + filename + ".csv", "w")
    json_file = json.load(in_file)
    all_bandwidth = json_file['intervals']
    usefull_bandwidth = all_bandwidth[30:]
    tmp_bandwith = 0
    for bw in usefull_bandwidth:
        tmp_bandwith += bw['sum']['bits_per_second']
        out_file.write(str(bw['sum']['bits_per_second']))
        out_file.write("\n")

    in_file.close()
    out_file.close()
    return tmp_bandwith / len(usefull_bandwidth)


def main(argv):
    path = argv[0]
    readBandwidth(path, "download")
    readBandwidth(path, "upload")


if __name__ == "__main__":
    main(sys.argv[1:])
