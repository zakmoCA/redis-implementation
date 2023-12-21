#include <iostream>
#include <sys/socket.h> // socket functions and structs
#include <netinet/in.h> // constants and structs needed for domain stuff
#include <unistd.h> // read write close etc
#include <cstring> // strlen
#include <cstdio> // printf, perror
#include <cstdlib> // exit


// introduction to sockets

static void handleClient(int connfd) {
  char rbuf[64] = {}; // this is a buffer to store client msg
  ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1); // read data from client into buffer
  if (n < 0) {
    msg("read() error");
  }
  printf("client says: %s\n", rbuf);

  char wbuf[] = "world"; // message to retun client
  write(connfd, wbuf, strlen(wbuf));
}

// error handling 
void msg(const char *message) {
  perror(message);  // print error message
  exit(EXIT_FAILURE); // exits program
}

// repeating msg() but better for clarity for now
void die(const char *message) {
  perror(message);
  exit(EXIT_FAILURE);
}

int main() {

  int fd = socket(AF_INET, SOCK_STREAM, 0);  // socket creation
  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  struct sockaddr_in addr = {};

  addr.sin_family = AF_INET;
  addr.sin_port = htons(1234);  // make sure to use network byte order
  addr.sin_addr.s_addr = htonl(INADDR_ANY);  // for binding to 0.0.0.0 wildcard address
  int rv = bind(fd, (struct sockaddr *)&addr, sizeof(addr)); // bind socket to the specified IP and port
  if (rv == -1) { // in C++ if (rv) will evaluate to true for any non-zero value ie. -1. 
  // if (rv) == true if it fails. will use below
    die("bind() failed");
  }

  // listen
  rv = listen(fd, SOMAXCONN); // listen on the socket for incoming connections
  // SOMAXCONN defines max length for the queue of pending connections https://stackoverflow.com/questions/18073483/what-do-somaxconn-mean-in-c-socket-programming
  if (rv) {
    die("listen() failed");
  }

  while (true) {
    // continuously accept new connections
    struct sockaddr_in client_addr = {}; // the client addr which will be filled in by accept()
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen); //accept new connection
    if (connfd < 0) {
      continue;   // skip rest of loop if accept fails
    }

    handleClient(connfd);
    close(connfd); // after handling
  }
      
  return 0;
}

