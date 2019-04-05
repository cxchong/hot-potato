#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

extern int hop_counter;

typedef struct potato_t {
  int num_hops;
  int curr;
  int tracelist[512];
} potato;


ssize_t send_all(int sockfd, const void *buf, size_t len) {
  size_t total = 0;
  while (1) {
    ssize_t num = send(sockfd, buf, len, 0);
    total += num;
    if (total == len) {
      break;
    }
  }
  return total;
}

int set_server(const char *port, struct addrinfo **p_host_info_list) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  // struct addrinfo *host_info_list;
  const char *hostname = NULL;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, p_host_info_list);
  if (status != 0) {
    fprintf(stdout, "set_server error: getaddrinfo\n");
    exit(EXIT_FAILURE);
  }

  socket_fd =
      socket((*p_host_info_list)->ai_family, (*p_host_info_list)->ai_socktype,
             (*p_host_info_list)->ai_protocol);
  if (socket_fd == -1) {
    fprintf(stdout, "set_server error: socket\n");
    exit(EXIT_FAILURE);
  } // if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

  status = bind(socket_fd, (*p_host_info_list)->ai_addr,
                (*p_host_info_list)->ai_addrlen);
  if (status == -1) {
    fprintf(stdout, "set_server error: bind\n");
    exit(EXIT_FAILURE);
  }
  status = listen(socket_fd, 100);
  if (status == -1) {
    fprintf(stdout, "set_server error: listen\n");
    exit(EXIT_FAILURE);
  }
  return socket_fd;
}

int set_client(struct addrinfo **p_host_info_list, const char *hostname,
               const char *port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, p_host_info_list);
  if (status != 0) {
    fprintf(stderr, "set_client error: getaddrinfo\n");
    exit(EXIT_FAILURE);
  }

  socket_fd =
      socket((*p_host_info_list)->ai_family, (*p_host_info_list)->ai_socktype,
             (*p_host_info_list)->ai_protocol);
  if (socket_fd == -1) {
    fprintf(stdout, "set_client error: socket\n");
    exit(EXIT_FAILURE);
  }

  status = connect(socket_fd, (*p_host_info_list)->ai_addr,
                   (*p_host_info_list)->ai_addrlen);
  if (status == -1) {
    fprintf(stdout, "set_client error: connect\n");
    exit(EXIT_FAILURE);
  }
  return socket_fd;
}
