obj-m += hello.o

all:
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) modules
	
clean: 
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) clean
	
#если мейкфайл начинается с маленькой буквы, то ошибка возникнет 
