#!/bin/bash

set -e

clear

if [ ! -d "../build" ]; then
    mkdir ../build && cd ../build && cmake .. && make
else
    cd ../build && rm -rf ../build/* && cmake .. && make
fi

cp -f ./kernel-de.conf ../package/etc/kernel-de/
cp -f ./kernel-de ../package/usr/bin
cp -f ../systemd/kde-collector.service ../package/etc/systemd/system/

sudo ./kernel-de ../configs/kernel-de.conf

exit 0
