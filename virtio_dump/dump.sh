#!/bin/bash
gcc dump_virtio_reg.c
sudo dpdk-devbind.py -b vfio-pci $1
sudo ./a.out $1
