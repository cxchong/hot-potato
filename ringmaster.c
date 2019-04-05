#include "potato.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stdout, "wrong argument number\n");
    exit(EXIT_FAILURE);
  }

  const char *port_num = argv[1];
  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);
  if (num_players <= 1) {
    fprintf(stdout, "wrong num_players\n");
    exit(EXIT_FAILURE);
  }
  if ((num_hops < 0) || (num_hops > 512)) {
    fprintf(stdout, "wrong num_hops\n");
    exit(EXIT_FAILURE);
  }
  srand((unsigned int)time(NULL));

  printf("Potato Ringmaster\n");
  printf("Players = %d\n", num_players);
  printf("Hops = %u\n", num_hops);

  struct addrinfo *host_info_list;
  int fd = 0;
  fd = set_server(port_num, &host_info_list);

  char hostnames[num_players][56];
  //  memset(hostnames, 0, sizeof(hostnames));
  int ids[num_players];
  // memset(ids, 0, sizeof(ids));
  int fds[num_players];
  // memset(fds, 0, sizeof(fds));
  char portnums[num_players][5];
  // memset(portnums, 0, sizeof(portnums));
  int rm_fd;
  char port[5];
  int id_temp[2];
  id_temp[1] = 0;

  int n_players[2];
  for (int i = 0; i < 2; i++) {
    memset(&n_players[i], 0, sizeof(n_players[i]));
  }
  n_players[0] = num_players;
  //  printf("num_plauers:%d\n", n_players[0]);
  // n_players[1] = 0;

  // int curr_count[2];
  // for (int i = 0; i < 2; i++) {
  // memset(&curr_count[i], 0, sizeof(curr_count[i]));
  //}
  // curr_count[1] = 0;
  for (int i = 0; i < num_players; i++) {
    id_temp[0] = i; ///!!!!!!!id
    ids[i] = i;
    struct sockaddr socket_addr;
    socklen_t address_len = sizeof(socket_addr);
    rm_fd = accept(fd, &socket_addr, &address_len); ///!!!!fd
    if (rm_fd == -1) {
      fprintf(stdout, "error: rm accept");
    }
    fds[i] = rm_fd;
    const struct sockaddr *socket_addr_ptr = &socket_addr;
    struct sockaddr_in *temp_in = (struct sockaddr_in *)socket_addr_ptr;
    char *r_hostname = inet_ntoa(temp_in->sin_addr); /// the hostname!!!
    char right_hostname[56];
    snprintf(right_hostname, 56, "%s", r_hostname);
    // printf("hostname: %s\n", r_hostname);
    // hostnames[i] = right_hostname;
    snprintf(hostnames[i], 56, "%s", right_hostname);

    ssize_t sz = recv(rm_fd, port, sizeof(port), 0); ///!!!!!!!portnum
    if (sz == -1) {
      fprintf(stdout, "error: recv portnum");
      exit(EXIT_FAILURE);
    }
    // printf("port:%s\n", port);
    snprintf(portnums[i], 6, "%s", port);
    sz = send_all(rm_fd, id_temp, sizeof(id_temp)); /////!!!!change size
    if (sz == -1) {
      fprintf(stdout, "error: send id");
      exit(EXIT_FAILURE);
    }

    sz = send_all(rm_fd, right_hostname, 56); //!!!
    if (sz == -1) {
      fprintf(stdout, "error: send right_hostname");
      exit(EXIT_FAILURE);
    }
    printf("Player %d is ready to play\n", i);
  }

  for (int i = 0; i < num_players; i++) {
    //  ssize_t sz = recv(fds[i], curr_count, sizeof(curr_count), MSG_WAITALL);
    // if (sz == -1) {
    // fprintf(stdout, "error: receive curr_count");
    // exit(EXIT_FAILURE);
    //}
    //}
    // printf("curr_count=%d\n", curr_count[0]);
    ssize_t mark = send_all(fds[i], n_players, 8);

    // printf("num_players size:%lu\n", sizeof(num_players));
    // printf("recheck num_players:%d\n", n_players[0]);
    if (mark == -1) {
      fprintf(stdout, "error: send num_players\n");
      exit(EXIT_FAILURE);
    }

    // for (int i = 0; i < num_players; i++) {
    mark = send_all(fds[i], hostnames, sizeof(hostnames));
    if (mark == -1) {
      fprintf(stdout, "error: send hostnames\n");
      exit(EXIT_FAILURE);
    }
    //}
    mark = send_all(fds[i], ids, sizeof(ids));
    if (mark == -1) {
      fprintf(stdout, "error: send ids\n");
      exit(EXIT_FAILURE);
    }
    
    mark = send_all(fds[i], portnums, sizeof(portnums));
    if (mark == -1) {
      fprintf(stdout, "error: send portnums\n");
      exit(EXIT_FAILURE);
    }
    //}
  }

  int start_player = rand() % num_players;
  //  int direction;
  printf("Ready to start the game, sending potato to player %d\n",
         start_player);
  potato container[2];
  for (int i = 0; i < 2; i++) {
    memset(&container[i], 0, sizeof(container[i]));
  }
  container[0].num_hops = num_hops;
  container[0].curr = 0;
  ssize_t size3 = send(fds[start_player], container, sizeof(container), 0);
  if (size3 == -1) {
    fprintf(stdout, "error: send potato\n");
    exit(EXIT_FAILURE);
  }
  fd_set rm_set;
  while (1) {
    FD_ZERO(&rm_set);
    for (int i = 0; i < num_players; i++) {
      FD_SET(fds[i], &rm_set);
    }

    if (select((fds[num_players - 1] + 1), &rm_set, NULL, NULL, NULL) == -1) {
      fprintf(stdout, "error:rm select\n");
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_players; i++) {
      ssize_t size4 = 0;
      if (FD_ISSET(fds[i], &rm_set)) {
        size4 = recv(fds[i], container, sizeof(container), MSG_WAITALL);

        if (size4 == -1) {
          fprintf(stdout, "error: isset\n");
          exit(EXIT_FAILURE);
        }
        int std;
        for (int i = 0; i < num_players; i++) {
          std = shutdown(fds[i], SHUT_RDWR);
          if (std != 0) {
            fprintf(stdout, "shutdown\n");
          }
        }

        printf("Trace of potato:\n");
        if (num_hops == 0) {
          printf("%d\n", container[0].tracelist[0]);
        } else {
          for (int i = 0; i < num_hops; i++) {
            if (i == (num_hops - 1)) {
              printf("%d\n", container[0].tracelist[i]);
            } else {
              printf("%d,", container[0].tracelist[i]);
            }
          }
        }
        freeaddrinfo(host_info_list);
        close(rm_fd);
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_SUCCESS;
}
