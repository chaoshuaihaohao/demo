cmd_/home/hao/demo/vfio/modules.order := {   echo /home/hao/demo/vfio/hello.ko; :; } | awk '!x[$$0]++' - > /home/hao/demo/vfio/modules.order
