import sys
from string import maketrans

if(len(sys.argv) < 3):
	print "Usage: python translate_suko.py inputMaze destinationFolder"
else:
	translationTable = maketrans(" .@$*", ".@*+!")

	suko = open(sys.argv[1])

	start = 0
	end = 0
	counter = 1

	lines = suko.readlines()

	while end < len(lines):
		for c in lines[start]:
			if not c in [' ', '@', '$', '*', '.', '#']:
				start += 1
				break

		while end < len(lines) and len(lines[end]) > 2: #"^@\n"
			end += 1

		puko = open(sys.argv[2] + "/" + str(counter) + ".txt", 'w')
		level = lines[start:end]
		for line in level:
			line = line.translate(translationTable)
			maxSize = len(max(level)) - 1
			for i in range(maxSize):
				if(i >= len(line) - 1):
					#sys.stdout.write(".")
					puko.write('.')
				else:
					#sys.stdout.write(line[i])
					puko.write(line[i])
			#sys.stdout.write('\n')
			puko.write('\n')
		puko.close()
		#sys.stdout.write('\n')

		counter += 1
		start = end
		end += 1
	suko.close()
