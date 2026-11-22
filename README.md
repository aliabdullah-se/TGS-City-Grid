# TGS CityGrid - Phase 1

## Build

g++ -std=c++17 -Wall -Icore -Iindex -Iengine engine/*.cpp -o citygrid

## Status

Implementation in progress.

## Hash collision method

Separate chaining with manually linked nodes is used.
The hash function is incidentID % capacity.
Deletion removes the hash node but not the Incident object.
After insertion makes the load factor greater than 0.75,
the table rehashes into the smallest prime capacity
greater than or equal to twice the previous capacity.
The table never shrinks.
