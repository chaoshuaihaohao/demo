cmd_/home/hao/demo/vfio/Module.symvers := sed 's/ko$$/o/' /home/hao/demo/vfio/modules.order | scripts/mod/modpost -m -a  -o /home/hao/demo/vfio/Module.symvers -e -i Module.symvers   -T -
