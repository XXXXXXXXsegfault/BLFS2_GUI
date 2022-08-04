#!/bin/sh
mkdir ./firmware
cd ./linux_firmware
make DESTDIR=../firmware install
