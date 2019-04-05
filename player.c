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
  if (argc != 3) {
    fprintf(stdout, "wrong argument number\n");
    exit(EXIT_FAILURE);
  }
  const char *hostname = argv[1];
  const char *port_num = argv[2];
  struct addrinfo *host_info_list;
  int player_fd = set_client(&host_info_list, hostname, port_num);
  int num_players;
  int temp_socket;

  struct addrinfo temp_info;
  struct addrinfo *temp_info_list;
  const char *temp_hostname = NULL;
  char port[5];
  for (int i = 6000; i <= 7000; i++) {
    snprintf(port, 5, "%d", i);
    // memset(&temp_info, 0, sizeof(temp_info));

    temp_info.ai_family = AF_UNSPEC;
    temp_info.ai_socktype = SOCK_STREAM;
    temp_info.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(temp_hostname, port, &temp_info, &temp_info_list);
    if (status != 0) {
      fprintf(stdout, "error: player left getaddrinfo\n");
      exit(EXIT_FAILURE);
    }
    temp_socket = socket(temp_info_list->ai_family, temp_info_list->ai_socktype,
                         temp_info_list->ai_protocol);
    if (temp_socket == -1) {
      fprintf(stdout, "error: left socket\n");
    }
    status =
        bind(temp_socket, temp_info_list->ai_addr, temp_info_list->ai_addrlen);
    if (status == -1) {
      freeaddrinfo(temp_info_list);
      continue;
    }
    status = listen(temp_socket, 1);
    if (status == 0) {
      break;
    }
  }

  ssize_t size = send_all(player_fd, port, sizeof(port)); //!!!!!!
  if (size == -1) {
    fprintf(stdout, "error: player send port\n");
    exit(EXIT_FAILURE);
  }
  // printf("1 send,port\n");
  // size = recv(player_fd, &num_players, sizeof(num_players), 0); ///!!!!!!!!
  int id_temp[2];
  for (int i = 0; i < 2; i++) {
    memset(&id_temp[i], 0, sizeof(id_temp[i]));
  }
  size = recv(player_fd, id_temp, sizeof(id_temp), MSG_WAITALL);

  int id = id_temp[0];
  // printf("id=%d\n", id);
  if (size == -1) {
    fprintf(stdout, "error: receive id\n");
    exit(EXIT_FAILURE);
  }
  // printf("1 recv,id_temp\n");

  char hostname_setup[56];
  size = recv(player_fd, hostname_setup, 56, MSG_WAITALL);
  hostname_setup[55] = 0;
  if (size == -1) {
    fprintf(stdout, "error: receive hostname\n");
    exit(EXIT_FAILURE);
  }

  int n_players[2];
  for (int i = 0; i < 2; i++) {
    memset(&n_players[i], 0, sizeof(n_players[i]));
  }
  // printf("sizeof(n_players):%lu\n", sizeof(n_players));
  size = recv(player_fd, n_players, 8, 0);

  // return 0;
  //}

  num_players = n_players[0];
  // printf("num_players:%d\n", num_players);
  if (size == -1) {
    fprintf(stdout, "error: receive num_players\n");
    exit(EXIT_FAILURE);
  }
  char hostnames[num_players][56];
  // memset(hostnames, 0, sizeof(hostnames));
  int ids[num_players];
  // int fds[num_players];
  char portnums[num_players][5];
  //  memset(portnums, 0, sizeof(portnums));
  // for (int i = 0; i < num_players; i++) {
  size = recv(player_fd, hostnames, sizeof(hostnames), MSG_WAITALL);
  // hostnames[i][55] = 0;
  // printf("hostnames[%d]:%s\n", i, hostnames[i]);
  if (size == -1) {
    fprintf(stdout, "error: receive hostnames\n");
    exit(EXIT_FAILURE);
  }
  //}
  size = recv(player_fd, ids, sizeof(ids), MSG_WAITALL);
  if (size == -1) {
    fprintf(stdout, "error: receive ids\n");
    exit(EXIT_FAILURE);
  }
  // size = recv(player_fd, fds, sizeof(fds), 0);
  // if (size == -1) {
  // fprintf(stdout, "error: receive fds\n");
  // exit(EXIT_FAILURE);
  //}
  // printf("sizeof(portnums):%lu\n", sizeof(portnums));
  // for (int i = 0; i < num_players; i++) {
  size = recv(player_fd, portnums, sizeof(portnums), MSG_WAITALL);
  // portnums[i][4] = 0;
  // printf("portnums[%d]:%s\n", i, portnums[i]);
  if (size == -1) {
    fprintf(stdout, "error: receive portnums\n");
    exit(EXIT_FAILURE);
  }
  //}
  // printf("portnums size:%lu\n", size);
  //  for (int i = 0; i < num_players; i++) {
  //    printf("hostnames:%s\n", hostnames[i]);
  //    printf("ids:%d\n", ids[i]);
  //    printf("fds:%d\n", fds[i]);
  //    printf("portnums:%s\n", portnums[i]);
  //  }
  printf("Connected as player %d out of %d total players\n", id, num_players);
  int right_fd;
  int left_fd;
  char *right_ip;
  char listen_port[5];
  int rightid;
  int leftid;

  if (id == 0) {
    leftid = num_players - 1;
    // printf("leftid:%d\n", leftid);
  } else {
    leftid = id - 1;
    // printf("leftid:%d\n", leftid);
  }
  // printf("test\n");
  if (id == num_players - 1) {
    rightid = 0;
    right_ip = hostnames[0];
    // printf("right_ip:%s\n", right_ip);
    snprintf(listen_port, 5, "%s", portnums[0]);
    // printf("listen_port:%s\n", portnums[0]);
  } else {
    rightid = id + 1;
    right_ip = hostnames[id + 1];
    // printf("right_ip:%s\n", right_ip);
    snprintf(listen_port, 5, "%s", portnums[id + 1]);
    // printf("listen_port:%s\n", portnums[id + 1]);
  }
  // printf("test\n");
  struct addrinfo *right_info_list;
  // printf("ip: %s,port: %s\n", right_ip, listen_port);
  right_fd = set_client(&right_info_list, right_ip, listen_port);
  left_fd = accept(temp_socket, NULL, NULL); //!!!!!!!!!!!!!!
  // printf("Connected as player %d out of %d total players\n", id,
  // num_players);

  fd_set player_set;
  int direction;
  int final_fd;
  srand((unsigned int)time(NULL) + id);
  potato container[2];
  for (int i = 0; i < 2; i++) {
    memset(&container[i], 0, sizeof(container[i]));
  }
  while (1) {
    FD_ZERO(&player_set);
    FD_SET(player_fd, &player_set);
    FD_SET(left_fd, &player_set);
    FD_SET(right_fd, &player_set);
    direction = rand() % 2;

    if (select((left_fd + 1), &player_set, NULL, NULL, NULL) == -1) {
      fprintf(stdout, "player select\n");
    }
    if (FD_ISSET(player_fd, &player_set)) {
      final_fd = player_fd;
    } else if (FD_ISSET(left_fd, &player_set)) {
      final_fd = left_fd;
    } else if (FD_ISSET(right_fd, &player_set)) {
      final_fd = right_fd;
    }

    size = recv(final_fd, container, sizeof(container), MSG_WAITALL);
    if (size == -1) {
      fprintf(stdout, "error:receive potato\n");
      exit(EXIT_FAILURE);
    }
    if (size == 0) {
      break;
    }
    container[0].tracelist[container[0].curr] = id;
    container[0].num_hops--;
    container[0].curr++;
    if (container[0].num_hops <= 0) {
      printf("I'm it\n");
      size = send_all(player_fd, container, sizeof(container));
      if (size == -1) {
        fprintf(stdout, "error: send potato\n");
        exit(EXIT_FAILURE);
      }
    } else {
      if (direction) {
        sleep(1);
        printf("Sending potato to %d\n", rightid);
        size = send_all(right_fd, container, sizeof(container));
        if (size == -1) {
          fprintf(stdout, "error: send right\n");
          exit(EXIT_FAILURE);
        }
      } else {
        sleep(1);
        printf("Sending potato to %d\n", leftid);
        size = send_all(left_fd, container, sizeof(container));
        if (size == -1) {
          fprintf(stdout, "error: send left");
          exit(EXIT_FAILURE);
        }
      }
    }
  }
  close(player_fd);
  close(right_fd);
  close(left_fd);
  close(temp_socket);

  freeaddrinfo(host_info_list);
  freeaddrinfo(temp_info_list);
  freeaddrinfo(right_info_list);
  return EXIT_SUCCESS;
}
