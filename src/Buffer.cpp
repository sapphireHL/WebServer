#include "Buffer.h"

#include <sys/uio.h>
#include <unistd.h>
#include <iostream>
using namespace std;

Buffer::~Buffer()
{
    //dtor
}

size_t Buffer::readFd(int fd)
{
    char extrabuf[65536];
    struct iovec vec[2];
    size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    int iovcnt = (writable < sizeof extrabuf)? 2:1;
    //int n = readv(fd, vec, iovcnt);
    int n = read(fd, begin()+writeIndex, writableBytes());
    if(n < 0){
        //error
        cerr << "read error"<<endl;
        cout << strerror(errno) << endl;
    }
    else if((size_t)n < writable){
        writeIndex += n;
    }
    else{
        writeIndex = buf.size();
        append(extrabuf, n - writable);
    }
    return n;
}
