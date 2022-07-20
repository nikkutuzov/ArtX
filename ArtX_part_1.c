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
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ev.h>

int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void Shutdown(int sockfd, int how);
void Close(int fd);

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);

int main() {
  // задаем номер порта
  unsigned short int port_number = 12345;

  // delfault loop
  struct ev_loop *loop = ev_default_loop(0);

  // создаем сокет: Internet-домен, TCP, протокол по умолчанию
  int sock = Socket(AF_INET, SOCK_STREAM, 0);

  // адрес
  struct sockaddr_in address;
  // занулим
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET; // Internet-домен
  address.sin_port = htons(port_number); // номер порта (htons - приводим
                                         // число с сетевому проядку байт short)
  address.sin_addr.s_addr = htonl(INADDR_ANY); // IP-адрес хоста (htonl - приводим
                                            // число к сетевому порядку байт long)
                                            // INADDR_ANY              0.0.0.0
                                            // INADDR_LOOPBACK       127.0.0.1
  // чтобы указать конкретный ip-адрес используется ip = inet_addr("10.0.0.1");
  // или новая ip = inet_pton(AF_INET, "10.0.0.1", &(address.sin_addr);

  // свяжем сокет с адресом
  Bind(sock, (struct sockaddr *)(&address), sizeof(address));

  // открываем слушающий TCP сокет
  Listen(sock, SOMAXCONN);  // длина очереди ожидания: 128 - для linux < 5.4
                            //                        4096 - для linux >= 5.4

  // инициализируем watcher
  struct ev_io w_accept;
  ev_io_init(&w_accept, accept_cb, sock, EV_READ);
  // запускаем
  ev_io_start(loop, &w_accept);

  while (1) { ev_loop(loop, 0); }

  return 0;
}

int Socket(int domain, int type, int protocol) {
int res = socket(domain, type, protocol);
  // проверяем
  if (res == -1) { // если ошибка
    perror("socket_err!"); // распечатываем не номер ошибки, а ее строковое значение
    exit(EXIT_FAILURE); // и выходим в этом случае
  }
  // елси ошибки нет, возвращаем дескриптор сокета
  return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  int res = bind(sockfd, addr, addrlen);
  if (res == -1) {
    perror("bind_err!");
    exit(EXIT_FAILURE);
  }
}

void Listen(int sockfd, int backlog) {
  int res = listen(sockfd, backlog);
  if (res == -1) {
    perror("listen_err!");
    exit(EXIT_FAILURE);;
  }
}

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  // создаем буфер
  char buffer[1024];

  // читаем
  // чтобы работало на windows меняем ssize_t на int
  ssize_t read_count = recv(watcher->fd, buffer, 1024, MSG_NOSIGNAL);
                              // MSG_NOSIGNAL - если соединение закрыто,
                              // не генерировать сигнал SIG_PIPE
  // обрабатываем ошибку
  if (read_count < 0) { return; }

  if (read_count == 0) { // соединение закрылось
    ev_io_stop(loop, watcher); // останавливаем watcher
    Shutdown(watcher->fd, SHUT_RDWR);
    Close(watcher->fd);
    free(watcher); // высвобождаем память
    return;
  } else { // иначе передаем
    send(watcher->fd, buffer, read_count, MSG_NOSIGNAL);
  }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int res = accept(sockfd, addr, addrlen);
  if (res == -1) {
    perror("accept_err!");
    exit(EXIT_FAILURE);
  }
  return res;
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  // принимаем соединение
  int client_socket = Accept(watcher->fd, NULL, 0);
                                        // NULL - struct sockaddr *
                                        // то есть ip-адрес и порт клиента
                                        // 0 - размер этой структуры
                                        // если указать NULL, 0 - игнорируем
                                        // информацию о клиенте
  // watcher на чтение
  // выделяем память чтобы при выходе из call_back область видимости
  // структуры сохранилась в КУЧЕ
  struct ev_io *w_client = (struct ev_io*) malloc(sizeof(struct ev_io));
  ev_io_init (w_client, read_cb, client_socket, EV_READ);
  ev_io_start(loop, w_client);
}

void Shutdown(int sockfd, int how) {
  int res = shutdown(sockfd, how);
  if (res == -1) {
    perror("shutdown_err!");
    exit(EXIT_FAILURE);
  }
}

void Close(int fd) {
  int res = close(fd);
  if (res == -1) {
    perror("close_err!");
    exit(EXIT_FAILURE);
  }
}
