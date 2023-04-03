#include <linux/module.h>/* Needed by all modules */
#include <linux/kernel.h>/* Needed for KERN_ALERT */
#include <linux/init.h>/*Needed for __init */

#include <linux/sched.h>



void mytest(void)
{
	struct task_struct *task0;
	struct task_struct *task1;
	struct pid *kpid;

	task0 = current;

	printk("pid = %d\n", task0->pid);

	kpid = find_get_pid(1);
	task1 = get_pid_task(kpid, PIDTYPE_PID);
	printk("pid = %d\n", task1->pid);

}




static int __init test_init(void)
{
	printk(KERN_ALERT"Hello world!\n");

	mytest();

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
