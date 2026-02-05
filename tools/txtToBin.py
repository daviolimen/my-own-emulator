import sys

if len(sys.argv) != 3:
    print("Expected 2 arguments: [input file] and [output file]")
    sys.exit(1)

infile, outfile = sys.argv[1], sys.argv[2]

with open(infile, "r") as fin, open(outfile, "wb") as fout:
    for line in fin:
        line = line.strip()
        if len(line) != 16: continue
        word = int(line, 2)
        fout.write(word.to_bytes(2, byteorder="little"))