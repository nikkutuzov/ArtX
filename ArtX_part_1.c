/*
 * Тестовое задание системного разработчика:
 *
 * 1) Реализовать на Си программу, использующую event loop (можно взять
 * любую библиотеку, желательно libev).
 * На вход программе передают 1 аргумент - номер порта.
 * На данном порте открыть слушающий TCP сокет, и на все входящие пакеты
 * отсылать их обратно в неизменном виде.
 * Тестировать функционал можно с помощью telnet.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ev.h>

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  // буфер
  char buffer[1024];

  // читаем
  ssize_t r = recv(watcher->fd, buffer, 1024, MSG_NOSIGNAL);

  // обрабатываем ошибку
  if (r < 0) { return; }

  if (r == 0) { // соединение закрылось
    ev_io_stop(loop, watcher);
    free(watcher);
    return;
  } else { // иначе передаем
    send(watcher->fd, buffer, r, MSG_NOSIGNAL);
  }
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  // принимаем соединение
  int client_socket = accept(watcher->fd, NULL, NULL);

  // watcher на чтение
  struct ev_io *w_client = (struct ev_io*) malloc(sizeof(struct ev_io));
  ev_io_init (w_client, read_cb, client_socket, EV_READ);
  ev_io_start(loop, w_client);
}

int main() {
  // задаем номер порта
  unsigned short int port_number = 12345;

  // delfault loop
  struct ev_loop *loop = ev_default_loop(0);

  // создаем сокет: Internet-домен, TCP, протокол по умолчанию
  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket!");
    exit(1);
  }

  // адрес
  struct sockaddr_in address;
  address.sin_family = AF_INET; // Internet-домен
  address.sin_port = htons(port_number); // номер порта
  address.sin_addr.s_addr = htonl(INADDR_ANY); // IP-адрес хоста

  // свяжем сокет с адресом
  if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror ("bind!");
    exit(2);
  }

  // открываем слушающий TCP сокет
  listen(sock, 1);

  // инициализируем watcher
  struct ev_io w_accept;
  ev_io_init(&w_accept, accept_cb, sock, EV_READ);
  ev_io_start(loop, &w_accept);

  while (1) { ev_loop(loop, 0); }

  return 0;
}
