#!/bin/bash

mpic++ -g -O3 -D_DOUBLE_ triad.cpp -o triad.p
mpic++ -g -O3 matrixMultiplication.cpp -o matMult.p
