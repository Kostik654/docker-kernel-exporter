#!/bin/bash

clear;

cd ../build && make && sudo ./kernel-de ../configs/kernel-de.conf

exit 0
