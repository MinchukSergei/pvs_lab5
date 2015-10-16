#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>



struct timer_list timer;


/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "delay", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */
//static struct timer_list exp_timer;
static long delay;

static void timer_function(unsigned long data)
{
    printk(KERN_INFO "Hello, World!\n");
    mod_timer(&timer, jiffies + delay * HZ);
    if (delay == 0) {
      del_timer(&timer);
    }
}
/*
 * The "delay" file where a static variable is read from and written to.
 */
static ssize_t sys_show(struct kobject *kobj, struct kobj_attribute *attr,
      char *buf)
{
  return sprintf(buf, "%ld\n", delay);
}

static ssize_t sys_store(struct kobject *kobj, struct kobj_attribute *attr,
       const char *buf, size_t count)
{
  
  kstrtol(buf, 10, &delay);
  if (delay == 0) {
      del_timer(&timer);
    }
  init_timer(&timer);
  timer.expires = jiffies;
  timer.data = 0;                     /* zero is passed to the timer handler */
  timer.function = timer_function;       /* function to run when timer expires */
  add_timer(&timer);
  return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute sys_attribute =
  __ATTR(delay, 0664, sys_show, sys_store);



/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
  &sys_attribute.attr,
  NULL, /* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
  .attrs = attrs,
};

static struct kobject *sys_kobj;

static int __init sys_init(void)
{
  int retval;
  
  /*
   * Create a simple kobject with the name of "kobject_example",
   * located under /sys/kernel/
   *
   * As this is a simple directory, no uevent will be sent to
   * userspace.  That is why this function should not be used for
   * any type of dynamic kobjects, where the name and number are
   * not known ahead of time.
   */
  
  
  sys_kobj = kobject_create_and_add("hello", kernel_kobj);
  if (!sys_kobj)
    return -ENOMEM;

  /* Create the files associated with this kobject */
  retval = sysfs_create_group(sys_kobj, &attr_group);
  if (retval)
    kobject_put(sys_kobj);

  return retval;
}

static void __exit sys_exit(void)
{
    del_timer(&timer);
    kobject_put(sys_kobj);
}

module_init(sys_init);
module_exit(sys_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Minchuk SG");