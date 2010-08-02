#!/bin/bash
time=10.0
for i in {1..193}
do
	echo "Running levels/extra-ksokoban/$i.txt..."
	#./solver "levels/extra-ksokoban/$i.txt" > "levels/extra-ksokoban/extra-ksokoban-puko-moves"
	#./solver "levels/extra-ksokoban/$i.txt" box-moves > "levels/extra-ksokoban/extra-ksokoban-box-moves"
	./solver "levels/extra-ksokoban/$i.txt" $time push-only component > "levels/extra-ksokoban/out-soko-component/extrak-$i-soko-component"
	#cat "levels/extra-ksokoban/extra-ksokoban-puko-moves-push-only"
	#./solver "levels/extra-ksokoban/$i.txt" box-moves push-only > "levels/extra-ksokoban/extra-ksokoban-box-moves-push-only"
done
