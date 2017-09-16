#include "rio.h"
#include "dbg.h"

Rio::Rio(int fd)
    : fd(fd){}

void Rio::remove_endl()const
{

}

//缓冲区无数据时，填充缓冲区
ssize_t Rio::_refull_buf()
{
    //缓冲区中的字符被读取殆尽
    debug("remain data=%d",remain_data);
    while (remain_data <= 0) {  /* Refill if buf is empty */
        //一次从fd中读取最大数量的字符填充满buffer
	remain_data = read(fd, buf, 
               sizeof(buf));
               //缓冲区仍然为空
    debug("从fd读取的字符数:%d",remain_data);
	if (remain_data < 0) {
        if (errno != EINTR) /* Interrupted by sig handler return */
            continue;
    }
    //已经没有字符可以读取了
	else if (remain_data == 0)  /* EOF */
	    return 0;
	else 
	    bufptr = buf; /* Reset buffer ptr */
    }
}

//缓冲区读基本函数，模拟系统read的读
ssize_t Rio::_read(int n)
{
    int cnt;
    debug("缓冲区不足");
    _refull_buf();
    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;          
    //缓冲区字符数不足
    if (remain_data < n)   
    //只读取缓冲区有的字符数目
    cnt=remain_data;
    //拷贝 到usrbuf
    memcpy(tmp, bufptr, cnt);
    //buffer游标后移动
    bufptr += cnt;
    remain_data -= cnt;
    //读取的字符数目
    return cnt;
}

string Rio::readline()
{
    int n, rc;
    result="";
    _is_empty=true;

    for (n = 1; n < MAX_LEN; n++)
    {
        if ((rc = _read(1)) == 1)
        {
            _is_empty=false;
            debug("tmp[0]=%c",tmp[0]);
            if (tmp[0] == '\n')
                break;
            result+=tmp[0];
        }
        else if (rc == 0)
        {
            if (n == 1)
                return result = "";
            else
                break; /* EOF, some data was read */
        }
        else
        {
            result = "";
            throw string("read error");
        }
    }
    return result;
}

void Rio::writeline(const string &_msg, int clientfd) const
{
    //剩余未写字符
    string msg=_msg;
    msg+="\n";
    size_t nleft = msg.size();
    ssize_t nwritten;
    const char *bufp = msg.data();

    while (nleft > 0)
    {
        if ((nwritten = write(clientfd, bufp, nleft)) <= 0)
        {
            if (errno == EINTR) /* Interrupted by sig handler return */
                nwritten = 0;   /* and call write() again */
            else
                throw string("write error");
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    //保证写了n个字符
}
