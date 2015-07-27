#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096
#define QUEUE_SIZE 10

void fatal(char * string) {
  printf("%s\n", string);
  exit(1);
}

void ack(int _sa) { 
  printf("in ack\n");
  write(_sa, "ok", 3);
  printf("returning ack\n");
}

void read_from_socket(int _sa) {
  char buf[BUF_SIZE];
  int bytes;

  printf("reading...\n");
  
  while (1) {
    bytes = read(_sa, buf, BUF_SIZE);
    if (bytes <= 0) break;
    write(1, buf, bytes);
  }

}

void write_to_socket(int _sa, char * filename) {
  char buf[BUF_SIZE];
  int bytes;
  int fd = open(filename, 'r');

  printf("writing...\n");
 
  if(fd < 0) fatal("problem opening file");

  while (1) {
    bytes = read(fd, buf, BUF_SIZE);
    if (bytes <= 0) break;
    write(_sa, buf, bytes);
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  int s, b, l, fd, sa, bytes, on = 1;

  char buf[BUF_SIZE];
  struct sockaddr_in channel;

  memset(&channel, 0, sizeof(channel));

  channel.sin_family = AF_INET;
  channel.sin_addr.s_addr = htonl(INADDR_ANY);
  channel.sin_port = htons(SERVER_PORT);

  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

  b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
  l = listen(s, QUEUE_SIZE);

  while (1) {
    printf("waiting for connection...\n");
    sa = accept(s, 0, 0);

    printf("someone connected\n");
    read (sa, buf, BUF_SIZE);
    
    printf("buf: %s\n", buf);

    if (!strcmp(buf, "-w")) {
      ack(sa);
      read_from_socket(sa);
      printf("continue w\n");
    } else if (!strcmp(buf, "-r")) {
      
      ack(sa);
      
      read(sa, buf, BUF_SIZE);

      write_to_socket(sa, buf);
      printf("continue r\n");
    }

    close(sa);
  }
}
