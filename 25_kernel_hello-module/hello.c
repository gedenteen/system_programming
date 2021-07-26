#include <linux/module.h> //для макросов
#include <linux/kernel.h> //pr_info()

int __init hello_init(void)
{
	pr_info("HELLO-MODULE: LOADED\n"); //вывод в лог-файл
	return 0;
}

void __exit hello_cleanup(void)
{
	pr_info("HELLO-MODULE: UNLOAD\n");
}

module_init(hello_init); //регистрация функций на запуск 
module_exit(hello_cleanup); //и завершение модуля
MODULE_LICENSE("GPLv2");
MODULE_DESCRIPTION("description");

