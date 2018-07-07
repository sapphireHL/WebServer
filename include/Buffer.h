#ifndef BUFFER_H
#define BUFFER_H

#include<string>
#include <string.h>

class Buffer
{
    public:
        static const int initialBufSize = 1024;
        explicit Buffer(int bufsize = initialBufSize) :buf(bufsize, ' '), readIndex(0), writeIndex(0) {};
        ~Buffer();

        void swap(Buffer& rhs)
        {
            buf.swap(rhs.buf);
            std::swap(readIndex, rhs.readIndex);
            std::swap(writeIndex, rhs.writeIndex);
        }

        size_t readableBytes() { return writeIndex - readIndex; }
        size_t writableBytes() { return buf.size() - writeIndex; }
        char* begin() { return &*buf.begin(); }
        char* data() { return begin() + readIndex; }
        char* beginWrite() { return begin() + writeIndex; }
        char* findCRLF()
        {
            if(readableBytes() == 0)
                return nullptr;
            int idx = buf.find("\r\n", readIndex);
            if((size_t)idx == std::string::npos) return nullptr;
            return begin()+idx;
        }

        // 提取所有的字节，作为一个string返回
        std::string retrieveAll()
        {
            std::string result(data(), readableBytes());
            readIndex = 0;
            writeIndex = 0;
            return result;
        }

        void append(std::string data)
        {
            int len = data.size();
            append(data.data(), len);
        }
        void append(const char* data, int len)
        {
            if(writableBytes() < (size_t)len){
                std::copy(data, data+writableBytes(), buf.begin()+writeIndex);
                for(int i=writableBytes()+1; i<len; ++i) buf += data[i];
            }
            else
                std::copy(data, data+strlen(data), buf.begin()+writeIndex);
            writeIndex += len;
        }

        std::string readAsString(size_t n)
        {
            std::string result(&*buf.begin()+readIndex, n);
            readIndex += n;
            if(readIndex == writeIndex){
                readIndex = 0;
                writeIndex = 0;
            }
            return result;
        }

        size_t readFd(int fd);

    private:
        std::string buf;
        int readIndex;
        int writeIndex;
};

#endif // BUFFER_H
