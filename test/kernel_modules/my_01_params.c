/*
 *  my_01_params.c - Пример передачи модулю аргументов командной строки.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Peter Jay Salzman");

static short int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "blah";

/* 
 * module_param(foo, int, 0000)
 * Первый параметр -- имя переменной,
 * Второй -- тип,
 * Последний -- биты прав доступа
 * для того, чтобы выставить в sysfs (если ненулевое значение) на более поздней стадии.
 */

module_param(myshort, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(myshort, "A short integer");
module_param(myint, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");
module_param(mylong, long, S_IRUSR);
MODULE_PARM_DESC(mylong, "A long integer");
module_param(mystring, charp, 0000);
MODULE_PARM_DESC(mystring, "A character string");

static int __init my_01_params_init(void)
{
        pr_info("my_01_params, init\n=============\n");
        pr_info("myshort is a short integer: %hd\n", myshort);
        pr_info("myint is an integer: %d\n", myint);
        pr_info("mylong is a long integer: %ld\n", mylong);
        pr_info("mystring is a string: %s\n", mystring);
        return 0;
}

static void __exit my_01_params_exit(void)
{
        pr_info("my_01_params, exit\n");
}

module_init(my_01_params_init);
module_exit(my_01_params_exit);
