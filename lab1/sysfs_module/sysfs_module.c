#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>

static struct kobject *team_kobj;

static ssize_t find_roots_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct task_struct *task = current;

    printk("find_roots sysfs opened by process %d\n", task->pid);

    while (task != NULL) {
        printk("id: %d, name: %s\n", task->pid, task->comm);

        if (task->pid == 1) {
            break;
        }
        task = task->parent;
    }

    return sprintf(buf, "%d\n", current->pid);
    return 0;
}

static struct kobj_attribute find_roots_attribute = __ATTR(find_roots, 0660, find_roots_show, NULL);

static int __init sysfs_module_init(void)
{
    int error = 0;

    team_kobj = kobject_create_and_add("team38994069", kernel_kobj);
    if (!team_kobj)
        return -ENOMEM;

    error = sysfs_create_file(team_kobj, &find_roots_attribute.attr);
    if (error) {
        printk("failed to create the find_roots file in /sys/kernel/team38994069\n");
    }

    return error;
}

static void __exit sysfs_module_exit(void)
{
    printk("Module un initialized successfully \n");
    kobject_put(team_kobj);
}

module_init(sysfs_module_init);
module_exit(sysfs_module_exit);

MODULE_LICENSE("GPL");
