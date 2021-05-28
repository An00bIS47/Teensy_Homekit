#!/bin/bash
python main.py -d 04:E9:E5:0C:D8:51 -p 031-45-712 -a T41 -c 2.12 -c 2.18 -C 2.10 "1" -e 2.10 -n 2 -z -f ./homekitStorage.json -N 10 -u -g `git log -1 --pretty="%H"` --log DEBUG
