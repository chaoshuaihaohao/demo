#include <linux/module.h>/* Needed by all modules */
#include <linux/kernel.h>/* Needed for KERN_ALERT */
#include <linux/init.h>/*Needed for __init */
#include <linux/ioport.h>


static int my_vfio_pci_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{

	struct resource *res;

        list_for_each_entry(res, &iomem_resource, sibling)
                if (strcmp(res->name, "0000:02:00.0") == 0)
                        break;

	printk("resource.start = %llx\n", res->start);
	printk("resource.end = %llx\n", res->end);


}


static const struct pci_device_id my_vfio_pci_table[] = {
        { PCI_DRIVER_OVERRIDE_DEVICE_VFIO(PCI_ANY_ID, PCI_ANY_ID) }, /* match all by default */
        {}
};

MODULE_DEVICE_TABLE(pci, my_vfio_pci_table);

static struct pci_driver my_vfio_pci_driver = {
        .name                   = "my_vfio-pci",
        .id_table               = my_vfio_pci_table,
        .probe                  = my_vfio_pci_probe,
        .remove                 = my_vfio_pci_remove,
//        .sriov_configure        = my_vfio_pci_sriov_configure,
//        .err_handler            = &my_vfio_pci_core_err_handlers,
        .driver_managed_dma     = true,
};


static int __init test_init(void)
{
	printk(KERN_ALERT"Hello world!\n");

	int ret = 0;
//	request_region();


	pci_register_driver(&my_vfio_pci_driver);



	return 0;
}

static void __exit test_exit(void)

{
	printk(KERN_ALERT"Goodbye world!\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_AUTHOR("Hao Chen <947481900@qq.com>");
MODULE_DESCRIPTION("ko templete");
MODULE_LICENSE("GPL v2");
