cmd_/home/hao/demo/iomem/iomem_resource.mod := printf '%s\n'   iomem_resource.o | awk '!x[$$0]++ { print("/home/hao/demo/iomem/"$$0) }' > /home/hao/demo/iomem/iomem_resource.mod
