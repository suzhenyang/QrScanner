#include "AgvSocketClient.h"

CAgvSocketClient::CAgvSocketClient(string server_ip,unsigned int server_port)
{
    ip = server_ip;
    port = server_port;
    fd = -1;
}

CAgvSocketClient::~CAgvSocketClient()
{

}

int CAgvSocketClient::SocketInit()
{
    if(fd < 0)
        fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof servaddr);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(fd,(struct sockaddr*) &servaddr, sizeof(struct sockaddr)) < 0)
    {
          perror("connect");
          return -2;
    }

    if(set_keep_alive() !=0 )
    {
        return -3;
    }

    int on = 1;
    setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, (void *)&on, sizeof(on));

    return 0;
}


int CAgvSocketClient::SocketWrite(const void *__buf, size_t __n)
{
    return write(fd,__buf,__n);
}

int CAgvSocketClient::SocketRead(void *__buf, size_t __nbytes)
{
    return read(fd,__buf,__nbytes);
}

int CAgvSocketClient::SocketClose()
{
    if(fd > 0)
    {
        if(close(fd) != 0)
        {
            perror("close");
            return -1;
        }
    }
    shutdown(fd,SHUT_RDWR);
    fd = -1;
    return 0;
}


int CAgvSocketClient::set_send_timeout(long seconds,long microseconds)
{

    struct timeval timeo = {1, 0};
    timeo.tv_sec = seconds;
    timeo.tv_usec = microseconds;

    socklen_t len = sizeof(timeo);

    if (setsockopt(fd, SOL_SOCKET,SO_SNDTIMEO, (const void*)&timeo, len) == -1)
    {
        perror("setsockopt");
        return -1;
    }

    return 0;

}

int CAgvSocketClient::set_recv_timeout(long seconds,long microseconds)
{

    struct timeval timeo;
    timeo.tv_sec = seconds;
    timeo.tv_usec = microseconds;

    socklen_t len = sizeof(timeo);

    if (setsockopt(fd, SOL_SOCKET,SO_RCVTIMEO, (const void*)&timeo, len) == -1)
    {
        perror("setsockopt");
        return -2;
    }

    return 0;

}


int CAgvSocketClient::set_keep_alive()
{

    int keepAlive = 1;
    int keepIdle = 4;
    int keepInterval = 3;
    int keepCout = 3;

    int res;
    res=setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&keepAlive,sizeof(int));
    if(-1 == res)
    {
        perror ("setsockopt");
        return -1;
    }

    res=setsockopt(fd,IPPROTO_TCP,TCP_KEEPIDLE,&keepIdle,sizeof(int));
    if(res<0)
    {
        perror ("setsockopt");
        return -2;
    }

    res=setsockopt(fd,IPPROTO_TCP,TCP_KEEPINTVL,&keepInterval,sizeof(int));
    if(res<0)
    {
        perror ("setsockopt");
        return -3;
    }

    res=setsockopt(fd,IPPROTO_TCP,TCP_KEEPCNT,&keepCout,sizeof(int));
    if(res<0)
    {
        perror ("setsockopt");
        return -4;
    }


    return 0;
}
