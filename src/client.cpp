#include "uti.hpp"
#include "rio.h"
#include "dbg.h"
#include<iostream>
using namespace std;
using namespace uti;
int main(int argc,char**argv)
{

    if (argc!=3)
    {
        log_err("usage: client <ip> <port>");
        exit(0);
    }

    


    int err;
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    check_exit(sockfd!=-1,"socket error");
    
    //epoll
    int epfd = epoll_create1(0);
    epoll_event ev, evs[EVENTARR];
    //add sockfd
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    //add STDIN_FILENO
    //fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); //设置非阻塞

    ev.data.fd=STDIN_FILENO;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

    sockaddr_in servaddr=get_client_struct(atoi(argv[2]),argv[1]);

    err=connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    check_exit(err==0,"connect error");
   
    char sendline[BUFFSIZE],recvline[BUFFSIZE];

    //Rio

    bool is_begin=true;

    while (1)
    {
        int nfds = epoll_wait(epfd, evs, EVENTARR, -1);
        for (int i = 0; i < nfds; ++i)
        {
            Rio rio(sockfd);
            //标准输入可读
            if (evs[i].data.fd==STDIN_FILENO)
            {
                string tmp="";
                getline(cin,tmp);
                tmp+="\n";
                debug("写到服务端:%s",tmp.c_str());
                rio.writeline(tmp,sockfd);
                cout<<">"<<flush;
            }
            //服务器端可读
            else if (evs[i].data.fd==sockfd)
            {
                cout << rio.readline() << endl<<">"<<flush;
            }
        }
    } 
    return 0;
}