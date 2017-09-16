#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <set>
#include "uti.hpp"
#include "rio.h"
#include "dbg.h"

using namespace uti;
using namespace std;

//描述符与用户名的对应关系
map<int, string> name_pool;

//已连接的描述符集合
set<int> all_fd;

char buf[BUFFSIZE];

void do_server(int fd);

int main(int argc,char** argv)
{

    if (argc!=2)
    {
        log_err("usage: server <port>");
        exit(0);
    }


    //open listenfd
    int listenfd = open_listenfd(atoi(argv[1]));
    set_nonblocking(listenfd);

    //epoll
    int epfd = epoll_create1(0);
    epoll_event ev, evs[EVENTARR];

    //bind listenfd to epoll
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    //init client struct
    sockaddr_in clientaddr;
    int clientfd;
    socklen_t clientlen;

    //event loop
    while (1)
    {
        int nfds = epoll_wait(epfd, evs, EVENTARR, -1);
        for (int i = 0; i < nfds; ++i)
        {
            //new client connect
            if (evs[i].data.fd == listenfd)
            {
                clientlen = sizeof(clientaddr);
                clientfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
                check_exit(clientfd >= 0, "accept error");
                set_nonblocking(clientfd);
                //bind clientfd to epoll
                ev.data.fd = clientfd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
                log_info("有新的客户端建立连接,fd=%d", clientfd);

                Rio rio(clientfd);
                //设置昵称
                rio.writeline("your name:",clientfd);
            }
            // new data from client
            else if ((clientfd=evs[i].data.fd) > 0)
            {
                debug("客户端fd=%d发来消息",clientfd);
                //rio包
                Rio rio(clientfd);

                //读取客户端发来的消息
                rio.readline();
                int len = rio.get_result().size();
                debug("len=%d",len);
                if (len > 0)
                {
                    //已经存在的用户
                    if (name_pool.find(clientfd) != name_pool.end())
                    {
                        string broad_cast=name_pool[clientfd]+":"+rio.get_result();
                        debug("已存在用户发来消息");
                        //回反给所有客户
                        for (auto fd : all_fd)
                        {
                            if (clientfd != fd)
                                rio.writeline(broad_cast, fd);
                        }
                    }
                    //新用户
                    else
                    {
                        string name = rio.get_result();
                        name_pool[clientfd] = name;
                        all_fd.insert(clientfd);

                        string result = "用户：" + name + " 加入聊天室";
                        log_info("%s", result.c_str());
                        for (auto fd : all_fd)
                            if (fd != clientfd)
                                rio.writeline(result, fd);
                        rio.writeline("------------欢迎来到聊天室-------------",clientfd);
                    }
                }
                else if (len == 0)
                {
                    if (!rio.is_empty())
                        continue;
                    log_info("用户：%s离开了聊天室", name_pool[clientfd].c_str());
                    close(clientfd);
                    evs[i].data.fd = -1;
                    name_pool.erase(clientfd);
                    all_fd.erase(clientfd);
                    break;
                }
                else if (len == EAGAIN)
                {
                    continue;
                }
            }
        }
    }
    return 0;
}

void do_server(int fd)
{
}