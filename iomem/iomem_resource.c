#include <linux/module.h>/* Needed by all modules */
#include <linux/kernel.h>/* Needed for KERN_ALERT */
#include <linux/init.h>/*Needed for __init */
#include <linux/resource_ext.h>


static struct resource my_res = {
    /* addr */
    .name  = "my_res",
#if 0
    .start = 0x2f200000,
    .end   = 0x2f201000,
#endif
    .start = 0x000e2000,
    .end   = 0x000e3000,
    .flags = IORESOURCE_MEM,
};

static int __init test_init(void)
{
	printk(KERN_ALERT"Hello world!\n");
	struct resource *parent, *new;
	int ret = 0;

	parent = &iomem_resource;
	new = &my_res;

	if (parent && insert_resource(parent, new)) {
		printk(KERN_ERR "failed to claim resource\n");
		ret = -EBUSY;
	}

	return ret;
}

static void __exit test_exit(void)

{
	struct resource *old;
	
	old = &my_res;

	remove_resource(old);
	printk(KERN_ALERT"Goodbye world!\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_AUTHOR("Hao Chen <947481900@qq.com>");
MODULE_DESCRIPTION("ko templete");
MODULE_LICENSE("GPL v2");
