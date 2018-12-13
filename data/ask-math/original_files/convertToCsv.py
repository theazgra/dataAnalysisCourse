from sys import argv


def main():

    textFile = argv[1]
    excludeTimeStamp = False
    if len(argv) > 2 and argv[2] == "no-timestamp":
        excludeTimeStamp = True

    #convertedFile = argv[2]
    #delimiter = argv[3] if len(argv) > 3 else ';'

    lines = []
    with open(textFile, 'r') as inFile:
        lines = inFile.readlines()

    with open(textFile.replace(".txt", ".csv"), 'w') as outFile:
        for line in lines:
            line = line.strip()

            if excludeTimeStamp:
                vals = line.split(" ")
                assert len(vals) == 3
                outFile.write("{};{}\n".format(vals[0], vals[1]))

            else:
                line = line.replace(" ", ";") + "\n"
                outFile.write(line)


if __name__ == "__main__":
    main()
