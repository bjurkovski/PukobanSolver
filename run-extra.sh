#!/bin/bash
for i in {1..193}
do
	echo "Running levels/extra-ksokoban/$i.txt..."
	./solver "levels/extra-ksokoban/$i.txt" > "levels/extra-ksokoban/out/$i.out" || rm "levels/extra-ksokoban/out/$i.out"
done
