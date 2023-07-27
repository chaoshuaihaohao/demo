#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <linux/vfio.h>
#include <linux/virtio_pci.h>
#include <linux/virtio_net.h>
#include <linux/pci_regs.h>
#include <linux/version.h>

#include <sys/mman.h>
#include <sys/ioctl.h>

#define VFIO_PATH "/dev/vfio/vfio"
#define DEVICE_ID "0000:60:00.3"	//The pci BDF of the device taken over by vfio.

#define BIT(nr)                 (1UL << (nr))
/*copy from virtio_net.c line 3180*/
#define VIRTNET_FEATURES \
        BIT(VIRTIO_NET_F_CSUM) | BIT(VIRTIO_NET_F_GUEST_CSUM) | \
        BIT(VIRTIO_NET_F_MAC) | \
        BIT(VIRTIO_NET_F_HOST_TSO4) | BIT(VIRTIO_NET_F_HOST_UFO) | BIT(VIRTIO_NET_F_HOST_TSO6) | \
        BIT(VIRTIO_NET_F_HOST_ECN) | BIT(VIRTIO_NET_F_GUEST_TSO4) | BIT(VIRTIO_NET_F_GUEST_TSO6) | \
        BIT(VIRTIO_NET_F_GUEST_ECN) | BIT(VIRTIO_NET_F_GUEST_UFO) | \
        BIT(VIRTIO_NET_F_MRG_RXBUF) | BIT(VIRTIO_NET_F_STATUS) | BIT(VIRTIO_NET_F_CTRL_VQ) | \
        BIT(VIRTIO_NET_F_CTRL_RX) | BIT(VIRTIO_NET_F_CTRL_VLAN) | \
        BIT(VIRTIO_NET_F_GUEST_ANNOUNCE) | BIT(VIRTIO_NET_F_MQ) | \
        BIT(VIRTIO_NET_F_CTRL_MAC_ADDR) | \
        BIT(VIRTIO_NET_F_MTU) | BIT(VIRTIO_NET_F_CTRL_GUEST_OFFLOADS) | \
        BIT(VIRTIO_NET_F_SPEED_DUPLEX) | BIT(VIRTIO_NET_F_STANDBY)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

struct virtio_pci_cap_cfg {
	u8 len;
	u8 type;
	u8 bar_index;
	u32 bar_offset;
	u32 bar_len;
};

struct vfio {
	int device;
	int group;
	int container;
};

struct vdpa_bar_info {
	uint64_t common;
	uint64_t device;
	struct virtio_pci_common_cfg *common_base;
	struct virtio_net_config *device_base;
	void *notify_base;

	u32 notify_off_multiplier;	/* Only used in notify cap */
};

struct pci_device {
	struct vfio vfio;
	char config[4096];
	void *mem_resource[6];		/* copy of bar space date */
	struct vfio_region_info reg[6];	/* bar space info */

	struct vdpa_bar_info vdpa;

} pdev;

#define VFIO_GET_REGION_ADDR(x) ((uint64_t) x << 40ULL)

static int pci_vfio_read_config(struct pci_device *pdev,
                    void *buf, size_t len, off_t offs)
{
        return pread(pdev->vfio.device, buf, len,
               VFIO_GET_REGION_ADDR(VFIO_PCI_CONFIG_REGION_INDEX) + offs);
}

#define TEST_BIT(a, b)  (b & (1ULL << a))

static void check_host_feature(uint64_t host_features)
{
	printf("%s: features = %lx\n", __func__, host_features);
/* Bit(0~31)*/
	printf("VIRTIO_NET_F_CSUM:[%s]\n", TEST_BIT(0, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_CSUM:[%s]\n", TEST_BIT(1, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_GUEST_OFFLOADS:[%s]\n", TEST_BIT(2, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_MTU:[%s]\n", TEST_BIT(3, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_MAC:[%s]\n", TEST_BIT(5, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_TSO4:[%s]\n", TEST_BIT(7, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_TSO6:[%s]\n", TEST_BIT(8, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_ECN:[%s]\n", TEST_BIT(9, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_UFO:[%s]\n", TEST_BIT(10, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_HOST_TSO4:[%s]\n", TEST_BIT(11, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_HOST_TSO6:[%s]\n", TEST_BIT(12, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_HOST_ECN:[%s]\n", TEST_BIT(13, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_HOST_UFO:[%s]\n", TEST_BIT(14, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_MRG_RXBUF:[%s]\n", TEST_BIT(15, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_STATUS:[%s]\n", TEST_BIT(16, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_VQ:[%s]\n", TEST_BIT(17, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_RX:[%s]\n", TEST_BIT(18, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_VLAN:[%s]\n", TEST_BIT(19, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_RX_EXTRA:[%s]\n", TEST_BIT(20, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_GUEST_ANNOUNCE:[%s]\n", TEST_BIT(21, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_MQ:[%s]\n", TEST_BIT(22, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_CTRL_MAC_ADDR:[%s]\n", TEST_BIT(23, host_features) ? "Yes":"No");

/* Bit(32~63)*/
	printf("VIRTIO_F_VERSION_1:[%s]\n", TEST_BIT(32, host_features) ? "Yes":"No");
	printf("VIRTIO_F_IOMMU_PLATFORM:[%s]\n", TEST_BIT(33, host_features) ? "Yes":"No");
	printf("VIRTIO_F_SR_IOV:[%s]\n", TEST_BIT(37, host_features) ? "Yes":"No");
	printf("VIRTIO_F_RING_RESET:[%s]\n", TEST_BIT(40, host_features) ? "Yes":"No");
	printf("VIRTIO_F_ADMIN_VQ:[%s]\n", TEST_BIT(41, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_NOTF_COAL:[%s]\n", TEST_BIT(53, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_HASH_REPORT:[%s]\n", TEST_BIT(57, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_RSS:[%s]\n", TEST_BIT(60, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_RSC_EXT:[%s]\n", TEST_BIT(61, host_features) ? "Yes":"No");
	printf("VIRTIO_NET_F_STANDBY:[%s]\n", TEST_BIT(62, host_features) ? "Yes":"No");
}

static void show_common_region(struct pci_device *pdev)
{
	printf("Dump VIRTIO_PCI_CAP_COMMON_CFG info:\n");

	struct virtio_pci_common_cfg *cfg;
	uint64_t host_features = 0, guest_features = VIRTNET_FEATURES;
	
	cfg = pdev->vdpa.common_base;

	cfg->device_feature_select = 0;
	printf("device_feature_select = 0x%08x\n", cfg->device_feature_select);
	printf("device_feature = 0x%08x\n", cfg->device_feature);

	host_features |= cfg->device_feature;

	cfg->device_feature_select = 1;
	printf("device_feature_select 1 = 0x%08x\n", cfg->device_feature_select);
	printf("device_feature 1 = 0x%08x\n", cfg->device_feature);
	host_features |= (uint64_t)cfg->device_feature << 32;

	/* Negosiate guest features */
	guest_features &= host_features;
	printf("Device init host features:\n");
	check_host_feature(host_features);
	
	/* Driver write negosiated features info guest_feature */
	printf("Driver write negosiated features info guest_feature\n");
	cfg->guest_feature_select = 0;
	cfg->guest_feature = (uint32_t)guest_features;
	printf("guest_feature_select:\t0x%08x\n", cfg->guest_feature_select);
	printf("guest_feature:\t0x%08x\n", cfg->guest_feature);
	guest_features = cfg->guest_feature;
	
	cfg->guest_feature_select = 1;
	cfg->guest_feature = (uint32_t)(guest_features >> 32);
	printf("guest_feature_select 1:\t0x%08x\n",
	       cfg->guest_feature_select);
	printf("guest_feature 1:\t0x%08x\n", cfg->guest_feature);
	guest_features |= (uint64_t)cfg->guest_feature << 32;
	
	printf("Device and driver negosiated features:\n");
	check_host_feature(guest_features);

	printf("msix_config:\t0x%04x\n", cfg->msix_config);
	printf("num_queues:\t0x%04x\n", cfg->num_queues);
	printf("device_status:\t0x%02x\n", cfg->device_status);
	printf("config_generation:\t0x%02x\n", cfg->config_generation);

	for (int qid = 0; qid < cfg->num_queues; qid++) {
//	for (uint16_t qid = 0; qid < 4; qid++) {
		cfg->queue_select = qid;
		printf("queue %u:>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", qid);
		printf("queue_select:\t%u\n", cfg->queue_select);
		printf("queue_size:\t%u\n", cfg->queue_size);
		printf("queue_msix_vector:\t0x%04x\n",
		       cfg->queue_msix_vector);
		printf("queue_enable:\t0x%04x\n", cfg->queue_enable);
		printf("queue_notify_off:\t0x%04x\n",
		       cfg->queue_notify_off);
		printf("queue_desc_lo:\t0x%08x\n", cfg->queue_desc_lo);
		printf("queue_desc_hi:\t0x%08x\n", cfg->queue_desc_hi);
		printf("queue_avail_lo:\t0x%08x\n",
		       cfg->queue_avail_lo);
		printf("queue_avail_hi:\t0x%08x\n",
		       cfg->queue_avail_hi);
		printf("queue_used_lo:\t0x%08x\n", cfg->queue_used_lo);
		printf("queue_used_hi:\t0x%08x\n", cfg->queue_used_hi);
	}
}

#if 1
static void show_notify_region(struct pci_device *pdev)
{
	printf("Dump VIRTIO_PCI_CAP_NOTIFY_CFG info:\n");

	// get total queue.
	struct virtio_pci_common_cfg *cfg;
	cfg = pdev->vdpa.common_base;
	void *notify_base = pdev->vdpa.notify_base;

	printf("num_queues:\t0x%04x\n", cfg->num_queues);
	for (int qid = 0; qid < cfg->num_queues; qid++) {
		cfg->queue_select = qid;
		printf("queue %u ~~~~~~~~~~~~~~~~\n", qid);

		printf("queue_notify_off:\t0x%04x\n",
		       cfg->queue_notify_off);
		printf("pdev->vdpa.notify_off_multiplier = %u\n", pdev->vdpa.notify_off_multiplier);
		u32 offset = cfg->queue_notify_off * pdev->vdpa.notify_off_multiplier;
		printf("offset = %u\n", offset);
		printf("notify addr = %p, notify[%d]=0x%x\n", notify_base+offset, qid, ((u32 *)notify_base)[qid]);
		//>>>test
#if 0
//		cfg->device_status = 0;
		printf("device_status:\t0x%02x\n", cfg->device_status);
		printf("queue_enable:\t0x%04x\n", cfg->queue_enable);
		cfg->queue_enable = 0;
		printf("queue_enable:\t0x%04x\n", cfg->queue_enable);
		printf("notify addr = %p, notify[%d]=0x%x\n", notify_base+offset, qid, ((u32 *)notify_base)[qid]);
		((u32 *)notify_base)[qid] = 1 << qid;
		printf("notify addr = %p, new notify[%d]=0x%x\n", notify_base+offset, qid, ((u32 *)notify_base)[qid]);
#endif
		//<<<test
	}


}
#endif

#if 0
static void
read_dev_config(struct pci_device *pdev, size_t offset, void *dst, int length)
{
	int i;
	uint8_t *p;
	uint8_t old_gen, new_gen;

	do {
		old_gen = pdev->vdpa.common->config_generation;

		p = dst;
		for (i = 0; i < length; i++)
			*p++ = *((uint8_t *) pdev->vdpa.device + offset + i);

		new_gen = pdev->vdpa.common->config_generation;
	} while (old_gen != new_gen);
}
#endif

static void show_device_region(struct pci_device *pdev)
{
	printf("Dump VIRTIO_PCI_CAP_DEVICE_CFG info:\n");

	struct virtio_net_config *net = pdev->vdpa.device_base;

	printf("MAC:\t0x %02x:%02x:%02x:%02x:%02x:%02x\n", net->mac[0],
	       net->mac[1], net->mac[2], net->mac[3], net->mac[4], net->mac[5]);

	printf("status = %u\n", net->status);

	printf("max_virtqueue_pairs = %u\n", net->max_virtqueue_pairs);
	printf("mtu = %u\n", net->mtu);
}


static void get_cap_info(struct pci_device *pdev)
{
	int ret;
	u8 pos;
	struct virtio_pci_cap cap;

	ret = pci_vfio_read_config(pdev, &pos, 1, PCI_CAPABILITY_LIST);
	if (ret < 0) {
		printf("failed to read pci capability list\n");
                return;
	}

	while (pos) {
		struct virtio_pci_cap virtio_cap;
		ret = pci_vfio_read_config(pdev, &cap, sizeof(cap), pos);
		if (ret < 0) {
			printf("failed to read cap at pos: %x\n", pos);
			break;
		}
		if (cap.cap_vndr != PCI_CAP_ID_VNDR)
			goto next;

		printf("cfg type: %u, bar: %u, offset: 0x%08x, "
                                "len: 0x%08x\n", cap.cfg_type, cap.bar,
                                cap.offset, cap.length);
		switch (cap.cfg_type) {
//Get the PCIe's virtio bar space
		case VIRTIO_PCI_CAP_COMMON_CFG:	//
			pdev->vdpa.common_base = pdev->mem_resource[cap.bar] + cap.offset;
			pdev->vdpa.common =  pdev->reg[cap.bar].offset + cap.offset;
			printf("common bar offset = 0x%lx\n", pdev->vdpa.common);
			show_common_region(pdev);
			break;
		case VIRTIO_PCI_CAP_NOTIFY_CFG:	//
#if 1
			pdev->vdpa.notify_base = pdev->mem_resource[cap.bar] + cap.offset;
			printf
			    ("Dump VIRTIO_PCI_CAP_NOTIFY_CFG info:>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//			get_virtio_cap_cfg_info(pdev,
//						&pdev->vdpa.notify,
//						pos);
			char *notify_cap = malloc(cap.cap_len);			
			ret = pci_vfio_read_config(pdev, notify_cap, cap.cap_len, pos);
			if (ret < 0) {
				printf("failed to read cap at pos: %x\n", pos);
				break;
			}
			pdev->vdpa.notify_off_multiplier = *(u32 *)(notify_cap+0x10);
			printf("notify_off_multiplier = %u\n", pdev->vdpa.notify_off_multiplier);

#endif
			show_notify_region(pdev);
			break;
		case VIRTIO_PCI_CAP_ISR_CFG:	//
#if 0
			pdev->vdpa.isr_base = pdev->mem_resource[cap.bar] + cap.offset;
			printf("Dump VIRTIO_PCI_CAP_ISR_CFG info:\n");
			get_virtio_cap_cfg_info(pdev,
						&pdev->virtio_isr_cap,
						pos);
#endif
			break;
		case VIRTIO_PCI_CAP_DEVICE_CFG:	//
			pdev->vdpa.device_base = pdev->mem_resource[cap.bar] + cap.offset;
			pdev->vdpa.device = pdev->reg[cap.bar].offset + cap.offset;
			printf("device bar offset = 0x%lx\n", pdev->vdpa.device);
			show_device_region(pdev);
			break;
		case VIRTIO_PCI_CAP_PCI_CFG:	//
#if 0			
			get_virtio_cap_cfg_info(pdev,
						&pdev->virtio_pci_cap,
						pos);
#endif
			break;
		}
next:		
		pos = cap.cap_next;
	}

}

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

	/* IOMMU pdev.vfio.group is always the last token */
	*iommu_group_num = strtol(group_tok, NULL, 10);
	return 1;
}

void region_info_flags_test(u_int32_t flags)
{
	if (flags & VFIO_REGION_INFO_FLAG_READ)
		printf("Region supports read\n");
	if (flags & VFIO_REGION_INFO_FLAG_WRITE)
		printf("Region supports write\n");
	if (flags & VFIO_REGION_INFO_FLAG_MMAP)
		printf("Region supports mmap\n");
	if (flags & VFIO_REGION_INFO_FLAG_CAPS)
		printf("Info supports caps\n");

}

int main(int argc, char *argv[])
{
	char *bdf;

	if (!argv[1]) {
		printf
		    ("Please input bdf of PCIe device. eg: ./a.out 0000:01:00.0\n");
		return -1;
	}
	bdf = argv[1];

	int i;
	struct vfio_group_status group_status =
	    {.argsz = sizeof(group_status) };
	struct vfio_iommu_type1_info iommu_info = {.argsz = sizeof(iommu_info)
	};
	struct vfio_iommu_type1_dma_map dma_map = {.argsz = sizeof(dma_map) };
	struct vfio_device_info device_info = {.argsz = sizeof(device_info) };
	int ret;

	/* Create a new pdev.vfio.container */
	pdev.vfio.container = open("/dev/vfio/vfio", O_RDWR);
	if (pdev.vfio.container < 0) {
		printf("Error pdev.vfio.container.\n");
		return -1;
	}

	/* Unknown API version */
	if (ioctl(pdev.vfio.container, VFIO_GET_API_VERSION) !=
	    VFIO_API_VERSION) {
		printf("Error: Unknown VFIO API version.\n");
		return -1;
	}

	/* Doesn't support the IOMMU driver we want. */
	if (!ioctl(pdev.vfio.container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU)) {
		printf("Error: Doesn't support the IOMMU driver we want.\n");
		return -1;
	}

	/* Get the device's pdev.vfio.group index */
	///sys/bus/pci/devices/0000\:60\:00.0/iommu_group
//	char *sys_path = "/sys/bus/pci/devices/" DEVICE_ID "/iommu_group";
	char sys_path[256];
	sprintf(sys_path, "/sys/bus/pci/devices/%s/iommu_group", argv[1]);
	printf("sys_path = %s\n", sys_path);
	int group_num;
	char group_name[20];
	memset(group_name, 0, sizeof(group_name));
	vfio_get_group_num(sys_path, &group_num);
	printf("group_num=%d\n", group_num);
	snprintf(group_name, sizeof(group_name), "/dev/vfio/%d", group_num);
	printf("group_name = %s\n", group_name);

	/* Open the pdev.vfio.group */
	pdev.vfio.group = open(group_name, O_RDWR);
	if (pdev.vfio.group < 0) {
		printf
		    ("Error: Group (%d). Is the device bond to vfio-pci drvier?\n",
		     pdev.vfio.group);
		return -1;
	}

	/* Test the pdev.vfio.group is viable and available */
	ioctl(pdev.vfio.group, VFIO_GROUP_GET_STATUS, &group_status);

	/* Group is not viable (ie, not all devices bound for vfio) */
	if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
		printf("Error: Group is not viable.\n");
		return -1;
	}

	/* Add the pdev.vfio.group to the pdev.vfio.container */
	ret =
	    ioctl(pdev.vfio.group, VFIO_GROUP_SET_CONTAINER,
		  &pdev.vfio.container);
	if (ret < 0) {
		printf("Error: VFIO_GROUP_SET_CONTAINER.\n");
		return -1;
	}

	/* Enable the IOMMU model we want */
	ret = ioctl(pdev.vfio.container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
	if (ret < 0) {
		printf("Error: VFIO_SET_IOMMU.\n");
		return -1;
	}

	/* Get addition IOMMU info */
	ret = ioctl(pdev.vfio.container, VFIO_IOMMU_GET_INFO, &iommu_info);
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

	ret = ioctl(pdev.vfio.container, VFIO_IOMMU_MAP_DMA, &dma_map);
	if (ret < 0) {
		printf("Error: VFIO_IOMMU_MAP_DMA.\n");
		return -1;
	}

	/* Get a file descriptor for the device */
	pdev.vfio.device =
	    ioctl(pdev.vfio.group, VFIO_GROUP_GET_DEVICE_FD, bdf);
//      device = ioctl(pdev.vfio.group, VFIO_GROUP_GET_DEVICE_FD, "0000:60:05.0 vf_token=cdc786f0-59d4-41d9-b554-fed36ff5e89f");
	if (pdev.vfio.device < 0) {
		printf("Error: VFIO_GROUP_GET_DEVICE_FD.(%d)\n",
		       pdev.vfio.device);
		return -1;
	}

	/* Test and setup the device */
	/* API VFIO_DEVICE_GET_INFO */
	ret = ioctl(pdev.vfio.device, VFIO_DEVICE_GET_INFO, &device_info);
	if (ret < 0) {
		printf("Error: VFIO_DEVICE_GET_INFO.\n");
		return -1;
	}
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,5,0)
	printf
	    ("device_info.flags=0x%x\t, device_info.num_regions=0x%x\t, device_info.num_irqs=0x%x\t, device_info.cap_offset=0x%x\n",
	     device_info.flags, device_info.num_regions, device_info.num_irqs,
	     device_info.cap_offset);
#else
	printf
	    ("device_info.flags=0x%x\t, device_info.num_regions=0x%x\t, device_info.num_irqs=0x%x\n",
	     device_info.flags, device_info.num_regions, device_info.num_irqs);
#endif

	region_info_flags_test(device_info.flags);

//Get the PCIe config space.
#define MIN(X, Y) ((X) <= (Y) ? (X) : (Y))
	struct vfio_region_info reg = {.argsz = sizeof(reg) };
	/* API VFIO_PCI_CONFIG_REGION_INDEX */
	reg.index = VFIO_PCI_CONFIG_REGION_INDEX;
	ret = ioctl(pdev.vfio.device, VFIO_DEVICE_GET_REGION_INFO, &reg);
	if (ret < 0) {
		printf
		    ("Error: VFIO_DEVICE_GET_REGION_INFO, REGION_INDEX=%u.\n",
		     reg.index);
	}
	printf
	    ("REGION_INDEX=%u, reg.flags=0x%x, cap_offset=0x%x, size=0x%llx, offset=0x%0llx\n",
	     reg.index, reg.flags, reg.cap_offset, reg.size, reg.offset);
/* Get a copy of config space */
#define PRINT_CONFIG_LEN 0x100
	ret = pread(pdev.vfio.device, pdev.config, MIN(0x1000, reg.size),	//PCIe 128
		    reg.offset);

	/* Dump region info */
	printf("Dump region info of : PCI_CONFIG\n");
	for (int of = 0x0; of < MIN(reg.size, PRINT_CONFIG_LEN); of++) {
		if (of % 16 == 0) {
			printf("offset: 0x%x\t", of);
		}
		printf("%02hhx ", pdev.config[of]);
		if (of % 16 == 15)
			printf("\n");
	}
//Get the bar0~bar5 space.
	for (int bar_idx = 0; bar_idx < 6; bar_idx++) {
		reg.index = bar_idx;
		ret =
		    ioctl(pdev.vfio.device, VFIO_DEVICE_GET_REGION_INFO, &reg);
		if (ret < 0) {
			printf
			    ("Error: VFIO_DEVICE_GET_REGION_INFO, REGION_INDEX=%u.\n",
			     reg.index);
		}
		printf
		    ("REGION_INDEX=%u, reg.flags=0x%x, cap_offset=0x%x, size=0x%llx, offset=0x%llx\n",
		     reg.index, reg.flags, reg.cap_offset, reg.size,
		     reg.offset);

/* Get a copy of bar region info */
		pdev.reg[bar_idx].index = reg.index;
		pdev.reg[bar_idx].flags = reg.flags;
		pdev.reg[bar_idx].cap_offset = reg.cap_offset;
		pdev.reg[bar_idx].size = reg.size;
		pdev.reg[bar_idx].offset = reg.offset;
/* Get a mmap of bar0~bar5 space */
		pdev.mem_resource[bar_idx] =
		    mmap(NULL, reg.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			 pdev.vfio.device, reg.offset);
		if (!pdev.mem_resource[bar_idx]) {
			printf("bar[%d] mmap failed!\n", bar_idx);
			return -1;
		}
	}

//Get the capabilities info of virtio.
	get_cap_info(&pdev);

	for (i = 0; i < device_info.num_irqs; i++) {
		struct vfio_irq_info irq = {.argsz = sizeof(irq) };

		irq.index = i;

		ret = ioctl(pdev.vfio.device, VFIO_DEVICE_GET_IRQ_INFO, &irq);

		/* Setup IRQs... eventfds, VFIO_DEVICE_SET_IRQS */
		if (ret < 0) {
			printf("Error: VFIO_DEVICE_GET_IRQ_INFO.\n");
			return -1;
		}
		printf("irq.flags=0x%x,irq.index=%u\t, irq.count=%u\n",
		       irq.flags, irq.index, irq.count);
	}

	/* Gratuitous device reset and go... */
	ioctl(pdev.vfio.device, VFIO_DEVICE_RESET);

	for (i = 0; i < 5; i++) {
		munmap(pdev.mem_resource[i], pdev.reg[i].size);
	}
}
