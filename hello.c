#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sorbat");
MODULE_DESCRIPTION("Hello Kernel Module");

static uint count = 1;  
module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "Number of times to print Hello, world!");

struct hello_event {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
    int i;
    struct hello_event *event;

    if (count == 0 || (count >= 5 && count <= 10)) {
        pr_warning("Hello Module: Warning! Parameter value is %u\n", count);
    }

    if (count > 10) {
        pr_err("Hello Module: Error! Parameter value %u is too large\n", count);
        return -EINVAL;
    }

    for (i = 0; i < count; i++) {
        event = kmalloc(sizeof(*event), GFP_KERNEL);
        if (!event) {
            pr_err("Hello Module: Memory allocation failed\n");
            return -ENOMEM;
        }

        event->time = ktime_get();
        list_add_tail(&event->list, &hello_list);

        pr_info("Hello, world! [%d]\n", i + 1);
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_event *event, *tmp;

    pr_info("Hello Module: Cleanup\n");

    list_for_each_entry_safe(event, tmp, &hello_list, list) {
        pr_info("Hello Module: Time: %lld ns\n", ktime_to_ns(event->time));
        list_del(&event->list);
        kfree(event);
    }
}

module_init(hello_init);
module_exit(hello_exit);
