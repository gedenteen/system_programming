# Описание

my_module.c - модуль ядра, который может "общаться" с user-space программой через файл устройства. Модуль создает файл устройства - /dev/my_module1, в который он может записывать и читать данные.



Примечание: почему-то невозможно загрузить модуль повторно (перегрузка ОС помогает). Ошибка: "insmod: ERROR: could not insert module my_module.ko: File exists".
