# Описание

Здесь программы с примерами мультиплексированного ввода-вывода. Информацию взял из книги Роберта Лава "Linux System programming".

**select.c** - программа с использованием системного вызова select(), который блокируется до тех пор, пока не станет доступен один из указанных файловых дескрипторов (далее ФД). Также надо перед каждым вызовом инициализировать переменные.

**poll.c** - программа с poll() из System V, смысл такой же. Используется один массив структур (struct pollfd), а не три набора ФД, как в select(). Не нужно заново инициализировать переменные; ядро обрабатывает обнуление поля revents, как необходимо.


