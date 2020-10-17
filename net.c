#define STATIC static

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define IS_WINDOWS 1
#elif __linux__
#include <netinet/in.h>

#define IS_WINDOWS 0
typedef uint64_t DWORD;
typedef uint64_t WSADATA;
#endif

struct sockaddr_in
make_addr_native(uint32_t ip, uint16_t port)
{
  struct sockaddr_in sa = {.sin_family = AF_INET,
                           .sin_addr.s_addr = htonl(ip),
                           .sin_port = htons(port)};

  return sa;
}

#define LOOPBACK 0x7f000001
#define RANDOM_PORT 0

STATIC void
cleanup(void)
{
  if (!IS_WINDOWS) return;

  WSACleanup();
}

STATIC void
exit_with_error(char *state, int code)
{
  if (IS_WINDOWS) {
    DWORD err = WSAGetLastError();
    printf("%s: [ WSAGetLastError %lu ]\n", state, err);
  } else {
    perror(state);
  }

  exit(code);
}

static char buffer[1400];
static struct sockaddr_in msg_sa;

int
main(int argc, char **argv)
{
  if (IS_WINDOWS) {
    WSADATA ws = {0};
    if (WSAStartup(MAKEWORD(2, 0), &ws) != 0) {
      perror("wsastartup");
      exit(1);
    }
    atexit(cleanup);
  }

  if (argc < 2)  // SERVER
  {
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in sa = make_addr_native(LOOPBACK, 2020);

    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
      exit_with_error("bind", 2);
    }

    do {
      size_t byte_count =
          recvfrom(s, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&msg_sa,
                   &(socklen_t){sizeof(msg_sa)});
      if (byte_count == -1) {
        exit_with_error("recvfrom", 3);
      }
      printf("%lu byte_count\n", byte_count);
      buffer[byte_count] = 0;
      puts(buffer);
    } while (true);
  } else {  // CLIENT
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in sa = make_addr_native(LOOPBACK, RANDOM_PORT);

    if (bind(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
      exit_with_error("bind", 2);
    }

    char *last = buffer + sizeof(buffer) - 1;
    char *write = buffer;
    for (int i = 1; i < argc; ++i) {
      uint64_t len = strlen(argv[i]);
      if (write + len < last) {
        strcpy(write, argv[i]);
        write += len;
        *write = ' ';
        write += 1;
      }
    }
    write -= (write != buffer);
    *write = 0;
    printf("sending %lu bytes\n", write - buffer);
    puts(buffer);

    struct sockaddr_in peer_sa = make_addr_native(LOOPBACK, 2020);
    if (!sendto(s, buffer, write - buffer, 0, (struct sockaddr *)&peer_sa,
                sizeof(peer_sa))) {
      exit_with_error("sendto", 3);
    }
  }

  return 0;
}
