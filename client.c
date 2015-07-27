#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define SERVER_PORT 12345
#define BUF_SIZE 4096

void fatal(char *string) {
  printf("%s\n", string);
  exit(1);
}

int main(int argc, char ** argv) {
  int c, s, bytes;
  char buf[BUF_SIZE];
  struct hostent *h;
  struct sockaddr_in channel;
  char * mode;
  int fd;

  if (argc != 4) fatal("argc error");

  mode = argv[1];

  h = gethostbyname(argv[2]);
  
  if (!h) fatal("gethostbyname error");

  printf("%s address is : %d.%d.%d.%d (%d oct)\n",h->h_name,
          h->h_addr[0] & 255,h->h_addr[1] & 255,h->h_addr[2],
          h->h_addr[3],h->h_length);

  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  if (s < 0) fatal("socket");
  else printf("socket handle: %d\n", s);

  memset(&channel, 0, sizeof(channel));

  channel.sin_family = AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);

  channel.sin_port = htons(SERVER_PORT);

  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  
  if (c < 0) fatal("connection error");
  
  write(s, mode, strlen(mode) + 1);
  
  printf("writing successfull\n");
  
  read(s, buf, BUF_SIZE);
  
  if (!strcmp(buf, "ok")) {
    switch (mode[1]) {
      case 'w':
        fd = open(argv[3], 'r');

        if(fd < 0) fatal("problem opening file");

        while (1) {
          bytes = read(fd, buf, BUF_SIZE);
          if (bytes <= 0) break;
          write(s, buf, bytes);
        }
        close(fd);
        break;
      case 'r':
         write(s, argv[3], strlen(argv[3])+1);
         while (1) {
          printf("\nreading bytes\n");
          bytes = read(s, buf, BUF_SIZE);
          if (bytes <= 0) break;
          write(1, buf, bytes);
        }
        break;
    }
  }
}

