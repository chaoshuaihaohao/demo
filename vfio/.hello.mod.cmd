cmd_/home/hao/demo/vfio/hello.mod := printf '%s\n'   hello.o | awk '!x[$$0]++ { print("/home/hao/demo/vfio/"$$0) }' > /home/hao/demo/vfio/hello.mod
