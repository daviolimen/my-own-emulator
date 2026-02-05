import sys

if len(sys.argv) != 3:
    print("Expected 2 arguments: [input file] and [output file]")
    sys.exit(1)

infile, outfile = sys.argv[1], sys.argv[2]

with open(infile, "rb") as fin, open(outfile, "w") as fout:
    while chunk := fin.read(2):
        inst = format(int.from_bytes(chunk, byteorder="little"), "016b")
        fout.write(inst + '\n')