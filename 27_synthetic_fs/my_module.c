#include <linux/module.h> //для макросов
#include <linux/kernel.h> //pr_info()
#include <linux/fs.h> //для чтения, записи в файл
#include <linux/rwlock.h> //механизм синхронизации
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

/// написано для ядра 5.8.0

#define MODULE_NAME "my_module"
static int major = 0;
static struct proc_dir_entry *proc_test_file = NULL;
static struct kobject *test_kobj = NULL;
static rwlock_t lock;
static char test_string[15] = "Hello!\n\0";

ssize_t my_read(struct file *fd, char __user *buff, size_t size, loff_t *ppos)
{
	size_t ret = 0;	
	
	read_lock(&lock);
	//см. libfs.c
	ret = simple_read_from_buffer(buff, size, ppos, test_string, 15);
	read_unlock(&lock);
	
	return ret;
}

ssize_t my_write(struct file *file, const char __user *buff,
                 size_t count, loff_t *ppos)
{
	size_t ret;
	
	if (count > 15)
		return -EINVAL;	
	
	write_lock(&lock);
	ret = simple_write_to_buffer(test_string, 15, ppos, buff, count);
	write_unlock(&lock);
	
	return ret;
}

ssize_t test_proc_read(struct file *fd, char __user *buff, 
                       size_t size, loff_t *off)
{
	size_t ret = 0;
	ret = simple_read_from_buffer(buff, size, off, test_string, 15);
	return ret;
} 

ssize_t test_proc_write(struct file *fd, const char __user *buff, 
                       size_t size, loff_t *off)
{
	size_t ret = 0;
	ret = simple_write_to_buffer(test_string, 15, off, buff, size); 
	return ret;
} 

/// Две хук-функции:
static ssize_t string_show(struct kobject *kobj, struct kobj_attribute *attr,
                           char *buf)
{
	size_t ret = 0;
	memcpy(buf, test_string, 15);
	ret = strlen(test_string);
	return ret;
}

static ssize_t string_store(struct kobject *kobj, struct kobj_attribute
                            *attr, char const *buf, size_t count)
{
	size_t ret = 0;
	memcpy(test_string, buf, count);
	ret = strlen(buf);
	return ret;
}

static const struct file_operations my_fops = {
	.owner =   THIS_MODULE,
	.write =   my_write,
	.read =    my_read,
};

static const struct proc_ops my_pops = {
	.proc_read = test_proc_read,
	.proc_write = test_proc_write,
};

/// Работа с /sys:
static struct kobj_attribute string_attribute = 
	__ATTR(test_string, 0664, string_show, string_store);
	
static struct attribute *attrs[] = {
	&string_attribute.attr,
	NULL,
}; 

static struct attribute_group attr_group = {
	.attrs = attrs,
};

int __init my_module_init(void)
{
	int ret = 0;
	pr_info("my_module: loaded\n"); //вывод в лог-файл
	rwlock_init(&lock);
	
	/// Регистрация мажорного номера для данного модуля ядра:
	major = register_chrdev(0, MODULE_NAME, &my_fops); 
	//ноль означает "пусть система сама выберет свободный мажорный номер"
	if (major < 0) {
		pr_err("my_module: error in register_chrdev()");
		return major;
	}
	pr_info("my_module: major = %d\n", major);
	
	/// Создание файла /proc/test:
	proc_test_file = proc_create("test_proc", 0666, NULL,  &my_pops);
	
	/// Создание kobject
	test_kobj = kobject_create_and_add("test_sys", kernel_kobj);
	if (!test_kobj)
		return -ENOMEM; 
	
	/// Создание файла /sys/kernel/test_sys/test_string:
	ret = sysfs_create_group(test_kobj, &attr_group);
	if (ret) {
		kobject_put(test_kobj);
		return ret;
	}
	
	return 0;
}

void __exit my_module_cleanup(void)
{
	unregister_chrdev(major, MODULE_NAME);
	proc_remove(proc_test_file);
	kobject_put(test_kobj);
	pr_info("my_module: unload\n");
}

module_init(my_module_init); //регистрация функций на запуск 
module_exit(my_module_cleanup); //и завершение модуля
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("description");

