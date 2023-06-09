#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/gfp.h>          // alloc_page
#include <linux/miscdevice.h>   // miscdevice misc_xxx
#include <linux/uaccess.h>      // copy_from/to_user 
#include <linux/dma-mapping.h>

#define DEMO_NAME "demo_dev"
#define PAGE_ORDER 5
#define MAX_SIZE (PAGE_SIZE << PAGE_ORDER)

static struct device *mydemodrv_device;
static void *device_buffer = NULL;
static dma_addr_t dma_handle;

static int demodrv_open(struct inode *inode, struct file *file)
{
   struct mm_struct *mm = current->mm;
   int major = MAJOR(inode->i_rdev);
   int minor = MINOR(inode->i_rdev);

   printk("%s: major=%d, minor=%d\n", __func__, major, minor);
 
   printk("client: %s (%d)\n", current->comm, current->pid);
   printk("code  section: [0x%lx   0x%lx]\n", mm->start_code, mm->end_code);
   printk("data  section: [0x%lx   0x%lx]\n", mm->start_data, mm->end_data);
   printk("brk   section: s: 0x%lx, c: 0x%lx\n", mm->start_brk, mm->brk);
   printk("mmap  section: s: 0x%lx\n", mm->mmap_base);
   printk("stack section: s: 0x%lx\n", mm->start_stack);
   printk("arg   section: [0x%lx   0x%lx]\n", mm->arg_start, mm->arg_end);
   printk("env   section: [0x%lx   0x%lx]\n", mm->env_start, mm->env_end);

   return 0;
}

static int demodrv_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t
demodrv_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int actual_readed;
    int max_read;
    int need_read;
    int ret;
    max_read = PAGE_SIZE - *ppos;
    need_read = max_read > count ? count : max_read;
    if (need_read == 0)
        dev_warn(mydemodrv_device, "no space for read");

    ret = copy_to_user(buf, (char *)device_buffer + *ppos, need_read);
    if (ret == need_read)
        return -EFAULT;
    actual_readed = need_read - ret;
    *ppos += actual_readed;

    printk("%s actual_readed=%d, pos=%lld\n", __func__, actual_readed, *ppos);
    return actual_readed;
}

static ssize_t
demodrv_write(struct file *file, const char __user *buf, size_t count,
              loff_t *ppos)
{
    int actual_written;
    int max_write;
    int need_write;
    int ret;
    max_write = PAGE_SIZE - *ppos;
    need_write = max_write > count ? count : max_write;
    if (need_write == 0)
        dev_warn(mydemodrv_device, "no space for write");

    ret = copy_from_user((char *)device_buffer + *ppos, buf, need_write);
    if (ret == need_write)
        return -EFAULT;
    actual_written = need_write - ret;
    *ppos += actual_written;

    printk("%s actual_written=%d, pos=%lld\n", __func__, actual_written, *ppos);
    return actual_written;
}

static int demodev_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct mm_struct *mm;
    unsigned long size;
    unsigned long pfn_start;
    void *virt_start;
    int ret;

    mm = current->mm;
    pfn_start = (dma_handle >> PAGE_SHIFT) + vma->vm_pgoff;
    virt_start = device_buffer + (vma->vm_pgoff << PAGE_SHIFT);

    /* 映射大小不超过实际物理页 */
    size = min(((1 << PAGE_ORDER) - vma->vm_pgoff) << PAGE_SHIFT,
               vma->vm_end - vma->vm_start);

    printk("phys_start: 0x%lx, offset: 0x%lx, vma_size: 0x%lx, map size:0x%lx\n",
           pfn_start << PAGE_SHIFT, vma->vm_pgoff << PAGE_SHIFT,
           vma->vm_end - vma->vm_start, size);

    if (size <= 0) {
        printk("%s: offset 0x%lx too large, max size is 0x%lx\n", __func__,
               vma->vm_pgoff << PAGE_SHIFT, MAX_SIZE);
        return -EINVAL;
    }

    // 外层vm_mmap_pgoff已经用信号量保护了 
    // down_read(&mm->mmap_sem);
    ret = remap_pfn_range(vma, vma->vm_start, pfn_start, size, vma->vm_page_prot);
    // up_read(&mm->mmap_sem);

    if (ret) {
        printk("remap_pfn_range failed, vm_start: 0x%lx\n", vma->vm_start);
    }
    else {
        printk("map kernel 0x%px to user 0x%lx, size: 0x%lx\n",
               virt_start, vma->vm_start, size);
    }

    return ret;
}


static loff_t demodev_llseek(struct file *file, loff_t offset, int whence)
{
    loff_t pos;
    switch(whence) {
    case 0: /* SEEK_SET */
        pos = offset;
        break;
    case 1: /* SEEK_CUR */
        pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END */
        pos = MAX_SIZE + offset;
        break;
    default:
        return -EINVAL;
    }
    if (pos < 0 || pos > MAX_SIZE)
        return -EINVAL;

    file->f_pos = pos; 
    return pos;
}

static const struct file_operations demodrv_fops = {
    .owner      = THIS_MODULE,
    .open       = demodrv_open,
    .release    = demodrv_release,
    .read       = demodrv_read,
    .write      = demodrv_write,
    .mmap       = demodev_mmap,
    .llseek     = demodev_llseek
};

static struct miscdevice mydemodrv_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEMO_NAME,
    .fops = &demodrv_fops,
};

static int __init demo_dev_init(void)
{
    int ret;

    ret = misc_register(&mydemodrv_misc_device);
    if (ret) {
        printk("failed to register misc device");
        return ret;
    }

    mydemodrv_device = mydemodrv_misc_device.this_device;

    printk("succeeded register misc device: %s\n", DEMO_NAME);

    if (!mydemodrv_device->coherent_dma_mask)
	    mydemodrv_device->coherent_dma_mask = DMA_BIT_MASK(32);
    device_buffer = dma_alloc_coherent(mydemodrv_device, MAX_SIZE, &dma_handle, GFP_KERNEL | __GFP_ZERO);
    if (!device_buffer) {
        printk("alloc dma coherent failed\n");
        return -ENOMEM;
    }
    printk("device_buffer physical address: 0x%llx, virtual address: %p, buffer size = 0x%lx\n",
           dma_handle, device_buffer, MAX_SIZE);

    return 0;
}


static void __exit demo_dev_exit(void)
{
    printk("removing device\n");
    
    dma_free_coherent(mydemodrv_device, MAX_SIZE, device_buffer, dma_handle);
    misc_deregister(&mydemodrv_misc_device);
}

module_init(demo_dev_init);
module_exit(demo_dev_exit);

MODULE_AUTHOR("Hao Chen <947481900@qq.com>");
MODULE_DESCRIPTION("ko templete");
MODULE_LICENSE("GPL v2");
