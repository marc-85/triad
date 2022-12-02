#!/bin/bash

for n in $(seq 2 30); do
	N=$((2**$n))
	echo $N $(taskset -c 0 ./triad.p $N)
done
