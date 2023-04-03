cmd_/home/hao/demo/iomem/modules.order := {   echo /home/hao/demo/iomem/iomem_resource.ko; :; } | awk '!x[$$0]++' - > /home/hao/demo/iomem/modules.order
