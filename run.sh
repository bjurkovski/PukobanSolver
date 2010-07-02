#!/bin/bash
for i in {0..11}
do
	echo "Running level $i..."
	./solver "levels/$i.txt" 1 $* > "levels/out/site-$i-puko-moves"
	cat "levels/out/site-$i-puko-moves"
	./solver "levels/$i.txt" 1 box-moves $* > "levels/out/site-$i-box-moves"
	cat "levels/out/site-$i-box-moves"
	#./solver "levels/$i.txt" push-only > "levels/out/site-$i-puko-moves-push-only"
	#./solver "levels/$i.txt" box-moves push-only > "levels/out/site-$i-box-moves-push-only"
done
