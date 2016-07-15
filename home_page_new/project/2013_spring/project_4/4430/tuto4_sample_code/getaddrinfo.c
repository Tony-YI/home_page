/* getaddrinfo.c -- demonstrating how to resolve hostnames into IPs
 *
 * Usage: ./getaddrinfo [domain] [port]
 * e.g.,
 * ./getaddrinfo google.com 80
 * ./getaddrinfo www.cse.cuhk.edu.hk 80
 */

#define _XOPEN_SOURCE 700  /* gai_strerror, getaddrinfo */
#include <arpa/inet.h>     /* inet_ntoa, ntohs */
#include <netdb.h>         /* gai_strerror, getaddrinfo */
#include <netinet/in.h>    /* inet_ntoa, struct sockaddr_in */
#include <stdio.h>         /* perror */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>    /* gai_strerror, getaddrinfo, inet_ntoa */
#include <sys/types.h>     /* gai_strerror, getaddrinfo */

int main (int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: %s [domain name] [port]\n", argv[0]);
    exit(1);
  }

  /* Resolve domain name */
  struct addrinfo hints, *results;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;        /* IPv4 */
  hints.ai_socktype = SOCK_STREAM;  /* TCP connection */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;
  int ret = getaddrinfo(argv[1], argv[2], &hints, &results);

  /* Error-handling */
  if (ret != 0) {
    if (ret == EAI_SYSTEM) {
      perror("getaddrinfo");
    } else {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
    }
    exit(1);
  }

  /* List results */
  struct addrinfo *p_results = results;
  for (int i=0; p_results!=NULL; p_results=p_results->ai_next) {
    struct sockaddr_in *sa = (struct sockaddr_in *)p_results->ai_addr;
    printf("[%d] %s:%hd\n", i++, inet_ntoa(sa->sin_addr), ntohs(sa->sin_port));
  }
  freeaddrinfo(results);
}

