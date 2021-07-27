my_01_params.c (на основе hello-5.c)
```bash
$ make
$ sudo insmod my_01_params.ko
$ sudo rmmod my_01_params
$ sudo insmod my_01_params.ko mystring="bebop" myshort=255
$ sudo rmmod my_01_params
$ cat /var/log/kern.log | grep my_01
```

