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
const char * port_num="4444";
struct addrinfo *host_info_list;
const char *hostname ="vcm-8003.vm.duke.edu";
 int player_fd = set_client(&host_info_list, hostname, port_num);
  printf("%d",player_fd);
  return EXIT_SUCCESS;}