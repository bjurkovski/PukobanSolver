#!/bin/bash
for i in {0..9}
do
	echo "Running level $i..."
	./solver "levels/0$i.txt" > "$i.out"
done
./solver "levels/10.txt" > 10.out

