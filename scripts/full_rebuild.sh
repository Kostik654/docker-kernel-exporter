#!/bin/bash

clear

if [ ! -d "../build" ]; then
    mkdir ../build && cd ../build && cmake .. && make && sudo ./kernel-de ../configs/kernel-de.conf
else
    cd ../build && rm -rf ./* && cmake .. && make && sudo ./kernel-de ../configs/kernel-de.conf
fi

exit 0
