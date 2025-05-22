#!/bin/bash

set -e

clear

bver=${1:-1.0}

cd ../build && make

cp -f ./kernel-de.conf ../package/etc/kernel-de/
cp -f ./kernel-de ../package/usr/bin
if [ ! -d "../releases/bin/${bver}" ]; then
    mkdir "../releases/bin/${bver}"
fi
cp -f ./kernel-de "../releases/bin/${bver}/kernel-de"
cp -f ../systemd/kde-collector.service ../package/etc/systemd/system/

sudo ./kernel-de kernel-de.conf

exit 0
