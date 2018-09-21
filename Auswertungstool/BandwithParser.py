import json
import sys


def bits_to_kbits(value, factor):
    value /= 1024
    if factor == "":
        factor = "K"
    elif factor == "K":
        factor = "M"
    elif factor == "M":
        factor = "G"
    elif factor == "G":
        factor = "T"
    elif factor == "T":
        factor = "mehr als T"
    return value, factor


def write_to_file(filename, value):
    file = open(filename, "w")
    file.write(value)


def main(argv):
    filename = argv[1]
    with open(filename, "r") as input_file:
        json_value = json.load(input_file)
        bandwidth = json_value['end']['sum']['bits_per_second']
        original_bandwidth = bandwidth
        factor = ""
        while bandwidth > 1024:
            bandwidth, factor = bits_to_kbits(bandwidth, factor)
        print("bandwith: " + "{0:0.3f}".format(bandwidth) + " " + factor + "bit/s")
        write_to_file(filename.split(".")[0] + "_Parsed .txt", str(original_bandwidth))


if __name__ == "__main__":
    main(sys.argv)
