
import os, sys

if (len(sys.argv) != 3):
#
	print("Usage: SplitIntoMultipleFiles.py [input_file_path] [split_string]");
	exit(1)
#

inputFilePath = sys.argv[1];
splitString = sys.argv[2];

inputFilePathNoExt, inputFileExt = os.path.splitext(inputFilePath);
inputFile = open(inputFilePath, "r");
inputContents = inputFile.read();
outputPieces = inputContents.split(splitString)

oIndex = 0;
for piece in outputPieces:
#
	while (len(piece) > 0 and (piece[0] == '\r' or piece[0] == '\n')): piece = piece[1:];
	while (len(piece) > 0 and (piece[-1] == '\r' or piece[-1] == '\n')): piece = piece[:-1];
	outputFilePath = ("%s%02d%s" % (inputFilePathNoExt, oIndex+1, inputFileExt));
	outputFile = open(outputFilePath, "w");
	outputFile.write(piece)
	outputFile.close();
	oIndex += 1;
#

print("Output %d files" % len(outputPieces));
