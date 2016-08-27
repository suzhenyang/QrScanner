#ifndef AGVSOCKETCLIENT_H
#define AGVSOCKETCLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <error.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/stat.h>

using namespace std;

class CAgvSocketClient
{
public:
    CAgvSocketClient(string server_ip,unsigned int server_port);
    ~CAgvSocketClient();

public:
    int SocketInit();
    int SocketWrite(const void *__buf, size_t __n);
    int SocketRead(void *__buf, size_t __nbytes);
    int SocketClose();

public:
    int set_keep_alive();
    int set_send_timeout(long seconds,long microseconds);
    int set_recv_timeout(long seconds,long microseconds);

    string ip;
    int port;
    int fd;
};

#endif // AGVSOCKETCLIENT_H
