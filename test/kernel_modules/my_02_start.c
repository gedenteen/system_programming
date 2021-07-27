/*
 *  start.c - Пример модуля, исходный текст которого размещен в нескольких файлах
 */

#include <linux/kernel.h>       /* Все-таки мы пишем код ядра! */
#include <linux/module.h>       /* Необходим для любого модуля ядра */

int init_module(void)
{
        pr_info("my_02_start: Hello, world - this is the kernel speaking\n");
        return 0;
}
