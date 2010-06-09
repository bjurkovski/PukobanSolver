import sys
from string import maketrans

if(len(sys.argv) < 3):
	print "Usage: python translate_suko.py inputMaze outputMaze"
else:
	translationTable = maketrans(" .@$", ".@*+")

	suko = open(sys.argv[1])
	puko = open(sys.argv[2], 'w')

	lines = suko.readlines()
	maxSize = len(max(lines)) - 1
	for line in lines:
		line = line.translate(translationTable)
		for i in range(maxSize):
			if(i >= len(line) - 1):
				puko.write('.')
			else:
				puko.write(line[i])
		puko.write('\n')
	puko.close()
	suko.close()
