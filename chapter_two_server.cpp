#include <iostream>
#include <sys/socket.h> // socket functions and structs
#include <netinet/in.h> // constants and structs needed for domain stuff
#include <unistd.h> // read write close etc
#include <cstring> // strlen
#include <cstdio> // printf, perror
#include <cstdlib> // exit


// introduction to sockets

static void do_something(int connfd) {
  char rbuf[64] = {};
  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
  if (n < 0) {
    msg("read() error");
  }
  printf("client says: %s\n", rbuf);

  char wbuf[] = "world";
  write(connfd, wbuf, strlen(wbuf));
}

// error handling 
void msg(const char *message) {
  perror(message);  // print error message
  exit(EXIT_FAILURE); // exits program
}

void die(const char *message) {

}

int main() {

  int fd = socket(AF_INET, SOCK_STREAM, 0);  // socket creation
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  struct sockaddr_in addr = {};

  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);  // make sure to use network byte order
  addr.sin_addr.s_addr = htonl(INADDR_ANY);  // for binding to 0.0.0.0 wildcard address
  int rv = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  if (rv) {
    die("bind()");
  }

  // listen
  rv = listen(fd, SOMAXCONN);
  if (rv) {
    die("listen()");
  }

  while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
      continue;   // error
    }

    do_something(connfd);
    close(connfd);
  }
      
  return 0;
}

