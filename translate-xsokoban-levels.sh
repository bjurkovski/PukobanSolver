#!/bin/bash
for i in {1..90}
do
	echo "Translating screen.$i..."
	python translate_soko.py xsokoban/screens/screen.$i levels/xsokoban/$i.txt
done
