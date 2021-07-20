# Описание

Нужно было собрать ядро Linux и убедиться, что оно работает.

# Выполнение

1. Скачал послденюю версию ядра с kernel.org
1. Почитал документацию, поизучал опции для файла .config через команду `make menuconfig`
1. Скачал кросскомпилятор gcc-4.9-arm-linux-gnueabihf
1. Создал готовый конфиг-файл (.config называется) через команду `make ARCH=arm multi_v7_defconfig` для соответсвующей платы (Arm Versatile Express board)
1. Создал образ ядра через `ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make zImage`
1. Создал DeviceTree-файлы через `ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- make dtbs`
1. Скопировал zImage и vexpress-v2p-ca9.dtb в отдельную папку
1. Запустил ядро на эмуляторе qemu через `QEMU_AUDIO_DRV=none qemu-system-arm -nographic -machine vexpress-a9 -kernel zImage -dtb vexpress-v2p-ca9.dtb -append "console=ttyAMA0"`. Здесь отключается использования графики и аудио, чтобы не возиться с ними, вывод определяется через интерфейс (наверное).
1. Создал простую программу для init-процесса. Статически (чтобы не возиться с динамикой) ее скомпилировал. Добавил в архив следующей командой: `echo init | cpio -o -H newc | gzip > initramfs.cpio.gz`.
1. Запустил ядро с этим архивом: `QEMU_AUDIO_DRV=none qemu-system-arm -nographic -M vexpress-a9 -kernel zImage -initrd initramfs.cpio.gz -dtb vexpress-v2p-ca9.dtb -append "console=ttyAMA0"`
1. Чтобы собрать файловую систему, можно использовать busybox:
   1. `ARCH=arm make defconfig`
   1. `ARCH=arm make menuconfig`. 
   1. Включил опцию "Build static binary" и для "Cross compiler prefix" указал "arm-linux-gnueabihf-".
   1. `ARCH=arm make -j4`
   1. `make install`
   1. `cd _install`
   1. `find . | cpio -o -H newc | gzip > initramfs.cpio.gz`
1. Заменил файл initramfs.cpio.gz и запустил через `QEMU_AUDIO_DRV=none qemu-system-arm -nographic -M vexpress-a9 -kernel zImage -initrd initramfs.cpio.gz -dtb vexpress-v2p-ca9.dtb -append "console=ttyAMA0 rdinit=/bin/ash"`. Здесь указывается программа ash для init-процесса. Командная строка в эмуляторе работает.
