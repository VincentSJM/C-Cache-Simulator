#!/bin/bash
make clean
make cacheSim
sleep 5
./cacheSim -t traces/gcc.trace -s 16 -w 1 -l 32
sleep 5
./cacheSim -t traces/gcc.trace -s 32 -w 2 -l 64
sleep 5
./cacheSim -t traces/gcc.trace -s 32 -w 8 -l 128 -lru
sleep 5
./cacheSim -t traces/gcc.trace -s 32 -w 512 -l 64
sleep 5
./cacheSim -t traces/gzip.trace -s 16 -w 1 -l 32
sleep 5
./cacheSim -t traces/gzip.trace -s 32 -w 2 -l 64
sleep 5
./cacheSim -t traces/mcf.trace -s 16 -w 1 -l 32
sleep 5
./cacheSim -t traces/mcf.trace -s 32 -w 2 -l 64
sleep 5
./cacheSim -t traces/swim.trace -s 16 -w 1 -l 32
sleep 5
./cacheSim -t traces/swim.trace -s 32 -w 2 -l 64
sleep 5
./cacheSim -t traces/twolf.trace -s 16 -w 1 -l 32
sleep 5
./cacheSim -t traces/twolf.trace -s 32 -w 2 -l 64
sleep 5
./cacheSim -t traces/mytrace.trace -s 1 -w 2 -l 32
sleep 5
