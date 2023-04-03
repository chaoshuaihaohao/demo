#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct virtio_net_config {
	int mac[6];
	int status;
};

struct virtio_net_config n = {
	.status = 1,
};

int main(int argc, char **arv)
{
	struct virtio_net_config net_cfg;

	memset(&net_cfg, 0, sizeof(net_cfg));
	net_cfg.status = 1;
	printf("status = %d\n", net_cfg.status);


	memcpy(&net_cfg, &n, 6);
	printf("net status = %d\n", net_cfg.status);

}
