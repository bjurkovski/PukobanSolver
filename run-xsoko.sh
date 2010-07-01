#!/bin/bash
for i in {1..90}
do
	echo "Running levels/xsokoban/$i.txt..."
	#./solver "levels/xsokoban/$i.txt" > "levels/xokoban/out/xsokoban-$i-puko-moves"
	#./solver "levels/xsokoban/$i.txt" box-moves > "levels/xsokoban/out/xsokoban-$i-box-moves"
	./solver "levels/xsokoban/$i.txt" push-only > "levels/xsokoban/out/xsokoban-$i-puko-moves-push-only"
	#./solver "levels/xsokoban/$i.txt" box-moves push-only > "levels/xsokoban/out/xsokoban-$i-box-moves-push-only"
done
python generate_table.py levels/xsokoban/out table-xsokoban.csv
