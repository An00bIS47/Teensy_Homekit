#!/bin/bash
python main.py -d 04:E9:E5:0C:D8:51 -p 031-45-712 -a T41 -c 2.10 -c 2.13 -c 2.16 -c 2.18 -C 2.10 "1" -e 2.10 -e 2.13 -e 2.16 -n 150 -z -f ./homekitStorage.json -N 10 -g `git log -1 --pretty="%H"` -u

