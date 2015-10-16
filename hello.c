#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/timer.h>




/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */
static struct timer_list exp_timer;
static long foo;


static void do_something(unsigned long data)
{
    printk(KERN_INFO "Hello, World!\n");       
}
/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
      char *buf)
{
  return sprintf(buf, "%ld\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
       const char *buf, size_t count)
{
  kstrtol(buf, 10, &foo);
  while (1) {
  init_timer_on_stack(&exp_timer);
  exp_timer.expires = jiffies + foo * HZ;
  exp_timer.data = 0;
  exp_timer.function = do_something;
  add_timer(&exp_timer);
}
  //setup_timer(&my_timer, foo_show, 0);
  /* setup timer interval to 200 msecs */
  // while (1) {
  //    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000 * foo));
  // }
  return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute foo_attribute =
  __ATTR(foo, 0664, foo_show, foo_store);



/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
  &foo_attribute.attr,
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

static struct kobject *example_kobj;

static int __init example_init(void)
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
  example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
  if (!example_kobj)
    return -ENOMEM;

  /* Create the files associated with this kobject */
  retval = sysfs_create_group(example_kobj, &attr_group);
  if (retval)
    kobject_put(example_kobj);

  return retval;
}

static void __exit example_exit(void)
{
    del_timer(&exp_timer);
    kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Minchuk SG>");