obj-m += my_01_params.o
obj-m += my_02_startstop.o
my_02_startstop-objs := my_02_start.o my_02_stop.o

all:
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) modules
	
clean: 
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) clean

