#!/bin/bash

#if [ $1 == "recover" ];then
#	sudo dpdk-devbind.py -b virtio-pci 


gcc dump_virtio_reg.c
if [ ! -d /sys/module/vfio_pci ];then
	sudo modprobe vfio-pci
fi
sudo dpdk-devbind.py -b vfio-pci $1 --force
sudo ./a.out $1
