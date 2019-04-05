#include "potato.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int main(){
const char *port_num ="4444";
struct addrinfo *host_info_list;
  int rm_fd = 0;
  rm_fd = set_server(port_num, &host_info_list);
  int num_players=3;
  playerinfo info[3];
  struct sockaddr address[num_players];
   for (int i = 0; i < num_players; i++) {
    info[i].pinfo_id = i;
    info[i].ownaddr = address[i];
    struct sockaddr_storage socket_addr;
    socklen_t address_len = sizeof(socket_addr);

    info[i].socket_fd = accept(rm_fd, &info[i].ownaddr,
                               &(address_len)); 

    if (info[i].socket_fd == -1) {
      fprintf(stdout, "error: accept");
      exit(EXIT_FAILURE);
    }
    }

return EXIT_SUCCESS;}