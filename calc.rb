sum = 0.0; 10.times{ |i| sum += IO.read("#{i+1}.out").split("\n")[5].split()[0].to_i }; print sum / 10
10.times{ |i| print IO.read("#{i+1}.out").split("\n")[5].split()[0] + "\n" }

