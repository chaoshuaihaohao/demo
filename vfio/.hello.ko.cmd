cmd_/home/hao/demo/vfio/hello.ko := ld -r -m elf_x86_64 -z noexecstack --build-id=sha1  -T scripts/module.lds -o /home/hao/demo/vfio/hello.ko /home/hao/demo/vfio/hello.o /home/hao/demo/vfio/hello.mod.o;  true