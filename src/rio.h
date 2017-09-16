#ifndef RIO_H
#define RIO_H

#define BUF_SIZE 8192
#define MAX_LEN 1024
#include <sys/types.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <errno.h>
//#define DBGON
#include "dbg.h"
using std::string;


class Rio
{
public:
    Rio(int fd);
    void remove_endl()const;
    string readline(); //从缓冲区中读取一行
    void writeline(const string& msg,int clientfd) const; //写到客户端
    string get_result() const{return result;}
    int get_fd(){return fd;}
    bool is_empty(){return _is_empty;}

private:
    ssize_t _read(int n); //从内置缓冲区读取n个字符,底层读取函数
    ssize_t _refull_buf(); //read最大数量的字符填充缓冲区
private:
    int fd;
    string result="";
    bool _is_empty=true;

    int remain_data=0;
    char* bufptr;
    char buf[BUF_SIZE];
    char tmp[MAX_LEN];
};

#endif