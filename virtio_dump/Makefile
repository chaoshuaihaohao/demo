all: 3 4 io

test:
	./virtio-dump.sh 0000:00:03.0
3:
	./virtio-dump.sh 0000:03:00.0
4:
	./virtio-dump.sh 0000:04:00.0
vf:
	./virtio-dump.sh 0000:03:00.1
	./virtio-dump.sh 0000:03:00.2
	./virtio-dump.sh 0000:03:00.3
	./virtio-dump.sh 0000:03:00.4

io:
	 sudo dpdk-devbind.py -b virtio-pci 0000:03:00.0
	 sudo io -4 -p 0000:03:00.0 -b 0 0x100 -l 160
	 sudo dpdk-devbind.py -b virtio-pci 0000:04:00.0
	 sudo io -4 -p 0000:04:00.0 -b 0 0x100 -l 160
