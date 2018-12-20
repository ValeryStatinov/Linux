#!/bin/bash
dev="keyboard_1min_statistics"
sudo rm /dev/$dev

make remove
make clean