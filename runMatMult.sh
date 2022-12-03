#!/bin/bash

for n in $(seq 1 30); do
	N=$((2**$n))
	echo $N $(taskset -c 0 ./matMult.p $N)
done
