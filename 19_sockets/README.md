# Запуск
## для локальных сокетов
```
cd local
make 
./server.exe
./client.exe hello (в другом терминале)
```

## для сетевых сокетов
```
cd inet
make 
./server.exe
./client.exe (в другом терминале)
```

# Описание

Папка "local" содержит эхо-сервер и клиент для **локальных** сокетов (AF_LOCAL), там TCP протокол (SOCK_STREAM), то есть потоковая передача данных, в которой подтверждается получение данных.  

Папка "inet" содержит сервери и клиент на **сетевых** сокетах (AF_INET), там UDP протокол (SOCK_DGRAM), то есть датаграммная пердача данных, которая более быстрая, чем TCP, и менее надежная. Для обмена датаграммами не нужно устанавливать соединение.
