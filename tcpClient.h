#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define IPDEST "127.0.0.1"

#define PORT 5000    /* the port client will be connecting to */

#define MAXDATASIZE 100 /* max number of bytes we can get at once */

void* tcpClientThread(void* t);
void sendPattern(int, int pattern[3]);
