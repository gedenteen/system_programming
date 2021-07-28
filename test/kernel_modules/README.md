# Описание

Здесь примеры из книги "The Linux Kernel Module Programming Guide".

## Примеры модулей

my_01_params.c (на основе hello-5.c) - на вход можно подать параметры. Например, так:

```bash
$ make
$ sudo insmod my_01_params.ko
$ sudo rmmod my_01_params
$ sudo insmod my_01_params.ko mystring="bebop" myshort=255
$ sudo rmmod my_01_params
$ cat /var/log/kern.log | grep my_01
```



my_02_start.c и my_02_stop.c (на основе start.c и stop.c) - сборка модуля из нескольких файлов.

```bash
$ make 
$ sudo insmod my_02_startstop.ko
$ sudo rmmod my_02_startstop.ko
$ cat /var/log/kern.log | grep my_02

```

