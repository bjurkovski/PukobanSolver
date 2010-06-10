#!/bin/bash
for i in {0..9}
do
	echo "Running level $i..."
	./solver "levels/0$i.txt" > "levels/out/$i.out"
done
./solver "levels/10.txt" > levels/out/10.out
