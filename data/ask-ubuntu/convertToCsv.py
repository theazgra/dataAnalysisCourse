from sys import argv


def main():

    textFile = argv[1]
    #convertedFile = argv[2]
    #delimiter = argv[3] if len(argv) > 3 else ';'

    lines = []
    with open(textFile, 'r') as inFile:
        lines = inFile.readlines()

    with open(textFile.replace(".txt", ".csv"), 'w') as outFile:
        for line in lines:
            line = line.strip()
            line = line.replace(" ", ";") + "\n"
            outFile.write(line)


if __name__ == "__main__":
    main()
