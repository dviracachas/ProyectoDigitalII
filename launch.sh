#!/bin/bash

export PATH=$PATH:/home/davidviralo/Xilinx/Vivado/Vivado/2020.1/bin
export PATH=$PATH:/opt/litex/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14/bin

./load.py
echo 'Load done'
echo '================================'
cd firmware
make all
make clean
echo 'make done'
echo '================================'
cd .. 

sudo chmod 777 /dev/ttyUSB1
litex_term /dev/ttyUSB1 --kernel firmware/firmware.bin

