import sys
import dircache

if(len(sys.argv) < 3):
	print "Usage: python generate_table.py solutionsFolder outputTable"
else:
	output = sys.argv[2]
	out = open(output, 'w')
	out.write("Instancia,Tempo,Total Branching,Average Branching, Min Branching, Max Branching,States Visited,Puko Moves,Box Moves\n")

	dirpath = sys.argv[1]
	files = dircache.listdir(dirpath)
	for filename in files:
		f = open(dirpath+"/"+filename)
		lines = f.readlines();
	
		out.write(dirpath+"/"+filename+",")
		out.write(lines[0])

		f.close()
	out.close()
