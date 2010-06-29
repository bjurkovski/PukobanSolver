import sys
import dircache

if(len(sys.argv) < 3):
	print "Usage: python generate_table.py solutionsFolder outputTable"
else:
	output = sys.argv[2]
	out = open(output, 'w')
	out.write("Instancia,Tempo,Total Branching,Average Branching, Min Branching, Max Branching,States Visited,Solution Length\n")

	dirpath = sys.argv[1]
	files = dircache.listdir(dirpath)
	for filename in files:
		f = open(dirpath+"/"+filename)
		lines = f.readlines();
	
		out.write(filename)
		for l in lines:
			words = l.split()
			out.write(",")
			out.write(words[0])
		out.write("\n")

		f.close()
	out.close()
