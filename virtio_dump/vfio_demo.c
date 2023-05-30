#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/vfio.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#define VFIO_PATH "/dev/vfio/vfio"
#define DEVICE_ID "0000:61:00.0"				//The pci BDF of the device taken over by vfio.

static int vfio_get_group_num(const char *linkname, int *iommu_group_num)
{
	char filename[256];
	char *group_tok, *end;
	int ret;

	memset(filename, 0, sizeof(filename));

	ret = readlink(linkname, filename, sizeof(filename));

	/* if the link doesn't exist, no VFIO for us */
	if (ret < 0)
		return 0;

	printf("%s\n", filename);

	end = strtok(filename, "/");

	while (end != NULL) {
		group_tok = end;
		end = strtok(NULL, "/");
	}

	/* IOMMU group is always the last token */
	*iommu_group_num = strtol(group_tok, NULL, 10);
	return 1;
}

int main()
{
	int container, group, device, i;
	struct vfio_group_status group_status =
	    {.argsz = sizeof(group_status) };
	struct vfio_iommu_type1_info iommu_info = {.argsz = sizeof(iommu_info)
	};
	struct vfio_iommu_type1_dma_map dma_map = {.argsz = sizeof(dma_map) };
	struct vfio_device_info device_info = {.argsz = sizeof(device_info) };
	int ret;

	/* Create a new container */
	container = open("/dev/vfio/vfio", O_RDWR);
	if (container < 0) {
		printf("Error container.\n");
		return -1;
	}

	/* Unknown API version */
	if (ioctl(container, VFIO_GET_API_VERSION) != VFIO_API_VERSION) {
		printf("Error: Unknown VFIO API version.\n");
		return -1;
	}

	/* Doesn't support the IOMMU driver we want. */
	if (!ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU)) {
		printf("Error: Doesn't support the IOMMU driver we want.\n");
		return -1;
	}

	/* Get the device's group index *sys/bus/pci/devices/0000\:60\:00.0/iommu_group */
	char *sys_path = "/sys/bus/pci/devices/" DEVICE_ID "/iommu_group";
	int group_num;
	char group_name[20];
	memset(group_name, 0, sizeof(group_name));
	vfio_get_group_num(sys_path, &group_num);
	printf("group_num=%d\n", group_num);
	snprintf(group_name, sizeof(group_name), "/dev/vfio/%d", group_num);

	/* Open the group */
	group = open(group_name, O_RDWR);

	/* Test the group is viable and available */
	ioctl(group, VFIO_GROUP_GET_STATUS, &group_status);

	/* Group is not viable (ie, not all devices bound for vfio) */
	if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
		printf("Error: Group is not viable.\n");
		return -1;
	}

	/* Add the group to the container */
	ret = ioctl(group, VFIO_GROUP_SET_CONTAINER, &container);
	if (ret < 0) {
		printf("Error: VFIO_GROUP_SET_CONTAINER.\n");
		return -1;
	}

	/* Enable the IOMMU model we want */
	ret = ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
	if (ret < 0) {
		printf("Error: VFIO_SET_IOMMU.\n");
		return -1;
	}

	/* Get addition IOMMU info */
	ret = ioctl(container, VFIO_IOMMU_GET_INFO, &iommu_info);
	if (ret < 0) {
		printf("Error: VFIO_IOMMU_GET_INFO.\n");
		return -1;
	}
	printf
	    ("iommu_info.flags=0x%x, iommu_info.iova_pgsizes=0x%llx, iommu_info.cap_offset=0x%x\n",
	     iommu_info.flags, iommu_info.iova_pgsizes, iommu_info.cap_offset);

	/* Allocate some space and setup a DMA mapping */
	dma_map.vaddr =
	    (unsigned long long)mmap(0, 1024 * 1024, PROT_READ | PROT_WRITE,
				     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	dma_map.size = 1024 * 1024;
	dma_map.iova = 0;	/* 1MB starting at 0x0 from device view */
	dma_map.flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE;

	ret = ioctl(container, VFIO_IOMMU_MAP_DMA, &dma_map);
	if (ret < 0) {
		printf("Error: VFIO_IOMMU_MAP_DMA.\n");
		return -1;
	}

	/* Get a file descriptor for the device */
	device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, DEVICE_ID);
	if (device < 0) {
		printf("Error: VFIO_GROUP_GET_DEVICE_FD.\n");
		return -1;
	}

	/* Test and setup the device */
	ret = ioctl(device, VFIO_DEVICE_GET_INFO, &device_info);
	if (ret < 0) {
		printf("Error: VFIO_DEVICE_GET_INFO.\n");
		return -1;
	}
	printf
	    ("device_info.flags=0x%x\t, device_info.num_regions=0x%x\t, device_info.num_irqs=0x%x\t, device_info.cap_offset=0x%x\n",
	     device_info.flags, device_info.num_regions, device_info.num_irqs,
	     device_info.cap_offset);

	/* index 0~5: bar space; can read/write/mmap
	 * index 6  : rom space; 
	 * index 7  : config space; can read/write */
	for (i = 0; i < device_info.num_regions; i++) {
		struct vfio_region_info reg = {.argsz = sizeof(reg) };

		reg.index = i;

		/* Setup mappings... read/write offsets, mmaps
		 * For PCI devices, config space is a region */
		ret = ioctl(device, VFIO_DEVICE_GET_REGION_INFO, &reg);
#if 0
		if (ret < 0) {
			printf("Error: VFIO_DEVICE_GET_REGION_INFO.\n");
			return -1;
		}
#endif
		printf
		    ("index=%u, reg.flags=0x%x, cap_offset=0x%x, size=0x%llx, offset=0x%llx\n",
		     reg.index, reg.flags, reg.cap_offset, reg.size, reg.offset);
	}

	for (i = 0; i < device_info.num_irqs; i++) {
		struct vfio_irq_info irq = {.argsz = sizeof(irq) };

		irq.index = i;

		ret = ioctl(device, VFIO_DEVICE_GET_IRQ_INFO, &irq);

		/* Setup IRQs... eventfds, VFIO_DEVICE_SET_IRQS */
		if (ret < 0) {
			printf("Error: VFIO_DEVICE_GET_IRQ_INFO.\n");
			return -1;
		}
		printf("irq.flags=0x%x,irq.index=%u\t, irq.count=%u\n", irq.flags, irq.index, irq.count);
	}

	/* Gratuitous device reset and go... */
	ioctl(device, VFIO_DEVICE_RESET);
}


