#include <linux/module.h>/* Needed by all modules */
#include <linux/kernel.h>/* Needed for KERN_ALERT */
#include <linux/init.h>/* Needed for __init */
#include <linux/mm.h> /* Needed for follow_pte */
#include <linux/spinlock_types.h>

#include <asm-generic/io.h>

static unsigned long address = 0xffffffffb0800000;
static int pid = 14434;

static int __init test_init(void)
{

	int ret;
	struct pid *kpid;
	struct task_struct *task;
	char task_name[TASK_COMM_LEN] = {0};
        spinlock_t *ptl;
        pte_t *ptep;
	unsigned long pfn;
	//struct page *page;


	printk(KERN_ALERT"Hello world!\n");
	kpid = find_get_pid(pid);
	task = get_pid_task(kpid, PIDTYPE_PID);
	printk("task name = %s , task pid = %d\n", get_task_comm(task_name, task), task->pid);

	printk("pgd = 0x%p\n", task->mm->pgd);

        ret = follow_pte(task->mm, address, &ptep, &ptl);
        if (ret)
                return ret;
        pfn = pte_pfn(*ptep);
        pte_unmap_unlock(ptep, ptl);
//	page = pfn_to_page(pfn);
//	        va = page_address(page);
//        printk("device_buffer physical address: %lx\n, the str value is %s\n",
                //pfn << PAGE_SHIFT, (char*)(pfn << PAGE_SHIFT));
        printk("device_buffer physical address: 0x%lx\n", pfn << PAGE_SHIFT);
        printk("device_buffer physical address: 0x%lx\n", virt_to_phys(address));
	


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
