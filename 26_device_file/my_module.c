#include <linux/module.h> //для макросов
#include <linux/kernel.h> //pr_info()
#include <linux/fs.h> //для чтения, записи в файл
#include <linux/rwlock.h> //механизм синхронизации
#include <linux/device.h> //для создания файла устройства

/// написано для ядра 5.8.0

#define MODULE_NAME "my_module"
static int major = 0;
static rwlock_t lock;
static char test_string[15] = "Hello!\n\0";
static struct class *my_class; //для создания файла устройства

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

const struct file_operations my_fops = {
	.owner =   THIS_MODULE,
	.write =   my_write,
	.read =    my_read,
};

int __init my_module_init(void)
{
	/// все переменные должны быть в начале:
	struct class *mtc; //это для my_class
	struct device *my_dev; //это для файла устройства
	const int minor = 1; //это минорный номер

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
	
	/// Создание класса для файла устройства:
	/// (пример взял из mt_init() в mips_mt.c):
	mtc = class_create(THIS_MODULE, MODULE_NAME);
	if (IS_ERR(mtc))
		return PTR_ERR(mtc);
	my_class = mtc;
	
	/// Создание файла устройства /dev/my_module1
	my_dev = device_create(my_class, NULL, MKDEV(major, minor), NULL,
	                    "%s%d", MODULE_NAME, minor);
	if (IS_ERR(my_dev)) {
		pr_info("my_module: error in device_create()\n");
		return 1;
	}
	
	return 0;
}

void __exit my_module_cleanup(void)
{
	unregister_chrdev(major, MODULE_NAME);
	pr_info("my_module: unload\n");
}

module_init(my_module_init); //регистрация функций на запуск 
module_exit(my_module_cleanup); //и завершение модуля
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("description");

