#!/bin/bash

set -e

clear

cd ../build && make

cp -f ./kernel-de.conf ../package/etc/kernel-de/
cp -f ./kernel-de ../package/usr/bin
cp -f ../systemd/kde-collector.service ../package/etc/systemd/system/

sudo ./kernel-de kernel-de.conf

exit 0
