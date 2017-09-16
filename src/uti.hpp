//头文件，以及一些简单的包装函数、宏定义

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h>  //inet_pton
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
//#define DBGON
#include "dbg.h"
// #include <unp.h>

namespace uti
{

#define SERV_PORT 8888
#define LISTENQ 1024
#define EVENTARR 20
#define BUFFSIZE 1024

#define SA sockaddr


//获取客户端需要的结构体
sockaddr_in get_client_struct(int port, const char *addr)
{
    int err;
    sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    err = inet_pton(AF_INET, addr, &servaddr.sin_addr);
    check_exit(err > 0, "inet_pton error");
    return servaddr;
}

//获取服务端需要的结构体
sockaddr_in get_server_struct(int port)
{
    sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    return servaddr;
}

//打开监听套接字
int open_listenfd(int port)
{
    int err, listenfd;
    sockaddr_in servaddr = get_server_struct(SERV_PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    check_exit(listenfd != -1, "create listenfd error");
    err = bind(listenfd, reinterpret_cast<SA *>(&servaddr), sizeof(servaddr));
    check_exit(err == 0, "bind error");
    err = listen(listenfd, LISTENQ);
    check_exit(err == 0, "listen error");

    return listenfd;
}

void set_nonblocking(int sockFd)
{
    int opt;

    //获取sock原来的flag
    opt = fcntl(sockFd, F_GETFL);
    check_exit(opt >= 0, "fcntl(F_GETFL) fail.");

    //设置新的flag,非阻塞
    opt |= O_NONBLOCK;
    int err = fcntl(sockFd, F_SETFL, opt);
    check_exit(err >= 0, "fcntl(F_SETFL) fail.");
}

//基本echo服务
void echo_serv(int fd)
{
}
}