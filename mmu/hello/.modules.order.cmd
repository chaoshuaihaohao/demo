cmd_/home/hao/demo/mmu/hello/modules.order := {   echo /home/hao/demo/mmu/hello/hello.ko; :; } | awk '!x[$$0]++' - > /home/hao/demo/mmu/hello/modules.order
