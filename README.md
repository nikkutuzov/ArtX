#for ArtX

Тестовое задание системного разработчика:

1) Реализовать на Си программу, использующую event loop (можно взять
любую библиотеку, желательно libev).
На вход программе передают 1 аргумент - номер порта.
На данном порте открыть слушающий TCP сокет, и на все входящие пакеты
отсылать их обратно в неизменном виде.
Тестировать функционал можно с помощью telnet.

Сделал!

2) В программу добавить 1 поток (всего будет 2 потока включая 1 главный
поток).
В потоке должен быть свой event loop. Перенести TCP сокет в этот поток.
Приняв пакет, нужно переслать его на обработку в главный поток.
Главный поток должен что-то сделать с пакетом, например перевернуть все
данные в нем задом наперед.
После этого пакет передается обратно в поток и там отсылается обратно
клиенту.

Сделал, но не все. Поток со своим event_loop'ом сделал, окет перенес,
но вот с переносом пакета в основной поток и т.д. НЕ справился :(

3) Вместо порта на вход передаются имена входящего и выходящего сетевых
интерфейсов (например eth0, eth1).
В потоке нужно ловить все входящие UDP пакеты на входящем интерфейсе (в
сыром виде, с L2 заголовками).
Далее пакет также передается в основной поток, UDP payload
переворачивается задом наперед, и пакет передается обратно в поток на
отправку.
Отправка осуществляется тоже в сыром виде в выходящий интерфейс, с
формированием L2 заголовков, подсчетом сумм итд.

Пока непосильная задача!

--------------------------------------------
Пункт 3 - это программа максимум, если сложно, можно просто словами
расписать технические детали как бы ты это реализовал
