import os


def get_lines_from_file(filename):
    infile = open(filename, "r")
    lines = infile.readlines()
    infile.close()
    return lines


def get_new_outfile(in_filename):
    parts = in_filename.split('.')
    filename = "." + parts[1] + "_new." + parts[2]
    outfile = open(filename, "w")
    return outfile


def delete_zeros_and_negatives(filename):
    lines = get_lines_from_file(filename)
    outfile = get_new_outfile(filename)
    for line in lines:
        if line != "0\n" and line != "0" and line[0] != "-":
            outfile.write(line)
    outfile.close()


def remove_invalid_lines_from_signalfile(filename):
    lines = get_lines_from_file(filename)
    outfile = get_new_outfile(filename)
    for line in lines:
        parts = line.split(';')
        if len(parts) == 7:
            invalid = False
            for part in parts:
                if part == "" or part == "\n":
                    invalid = True
            if not invalid:
                outfile.write(line)
    outfile.close()


def calculate_reordering(filename):
    lines = get_lines_from_file(filename)
    linecounter = 0
    reordering_counter = 0
    for line in lines:
        if line != "\n":
            linecounter += 1
            if line != "0\n" and line != "0" and line != "":
                reordering_counter += 1

    outfile = get_new_outfile(filename)
    reordering = 100 / linecounter * reordering_counter
    outfile.write(str(reordering))
    outfile.close()


def calculate_duplication(filename):
    lines = get_lines_from_file(filename)
    linecounter = 0
    duplication_counter = 0
    for line in lines:
        if line != "\n":
            linecounter += 1
            if line != "0\n" and line != "1\n" and line != "0" and line != "1" and line != "":
                duplication_counter += 1

    outfile = get_new_outfile(filename)
    duplication = 100 / linecounter * duplication_counter
    outfile.write(str(duplication))
    outfile.close()


def main():
    for root, subdirs, files in os.walk('.'):
        for file in files:
            filepath = root + "/" + file
            if file == "downloadDelays.csv" or file == "uploadDelays.csv":
                delete_zeros_and_negatives(filepath)
            elif file == "Signal.csv":
                remove_invalid_lines_from_signalfile(filepath)
            elif file == "uploadReordering.txt" or file == "downloadReordering.txt":
                calculate_reordering(filepath)
            elif file == "uploadDuplication.txt" or file == "downloadDuplication.txt":
                calculate_duplication(filepath)


if __name__ == "__main__":
    main()
