cmd_/home/hao/demo/iomem/Module.symvers := sed 's/ko$$/o/' /home/hao/demo/iomem/modules.order | scripts/mod/modpost -m -a  -o /home/hao/demo/iomem/Module.symvers -e -i Module.symvers   -T -
