#!/bin/sh
cd openssl
./config -static
make -j6
