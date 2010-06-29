#!/bin/bash
for i in {1..193}
do
	echo "Running levels/extra-ksokoban/$i.txt..."
	./solver "levels/extra-ksokoban/$i.txt" push-only > "levels/extra-ksokoban/out-suko/$i-soko.out" || rm "levels/extra-ksokoban/out/$i.out"
done
