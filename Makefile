CXXFLAGS=	-Wall -I include -fexceptions -g -std=c++11
CC=		g++
LIBS=		-lpthread ./lib/libcpp_redis.a ./lib/libtacopie.a
OBJECTS=	main.o Acceptor.o Buffer.o Channel.o EventLoop.o EventLoopThread.o EventLoopThreadPool.o \
	 	HttpContext.o HttpResponse.o HttpServer.o HttpServerConfig.o InetAddress.o Poller.o \
		Socket.o TcpConnection.o TcpServer.o Thread.o ThreadPool.o Timer.o TimerQueue.o \
		Timestamp.o RedisClient.o
VPATH = include:src

server : $(OBJECTS) 
	$(CC) $(CXXFLAGS) -o server $(OBJECTS) $(LIBS) 

main.o : main.cpp 

Acceptor.o: ./src/Acceptor.cpp ./include/Acceptor.h

Buffer.o: ./src/Buffer.cpp ./include/Buffer.h

Channel.o: ./src/Channel.cpp ./include/Channel.h

EventLoop.o: ./src/EventLoop.cpp ./include/EventLoop.h ./include/Channel.h ./include/Poller.h \
	./include/TimerQueue.h ./include/MutexLockGuard.h

EventLoopThread.o: ./src/EventLoopThread.cpp ./include/EventLoopThread.h ./include/EventLoop.h ./include/MutexLockGuard.h

EventLoopThreadPool.o: ./src/EventLoopThreadPool.cpp ./include/EventLoopThreadPool.h ./include/EventLoopThread.h

HttpContext.o: ./src/HttpContext.cpp ./include/HttpContext.h ./include/Buffer.h

HttpResponse.o: ./src/HttpResponse.cpp ./include/HttpResponse.h ./include/Buffer.h

HttpServer.o: ./src/HttpServer.cpp ./include/HttpServer.h ./include/Buffer.h ./include/HttpResponse.h ./include/HttpContext.h ./include/HttpRequest.h

HttpServerConfig.o: ./src/HttpServerConfig.cpp ./include/HttpServerConfig.h

InetAddress.o: ./src/InetAddress.cpp ./include/InetAddress.h

Poller.o: ./src/Poller.cpp ./include/Poller.h ./include/Channel.h

Socket.o: ./src/Socket.cpp ./include/Socket.h

TcpConnection.o: ./src/TcpConnection.cpp ./include/TcpConnection.h ./include/Socket.h ./include/Channel.h

TcpServer.o: ./src/TcpServer.cpp ./include/TcpServer.h ./include/Acceptor.h ./include/EventLoopThreadPool.h

Thread.o: ./src/Thread.cpp ./include/Thread.h

ThreadPool.o: ./src/ThreadPool.cpp ./include/ThreadPool.h

Timer.o: ./src/Timer.cpp ./include/Timer.h

TimerQueue.o: ./src/TimerQueue.cpp ./include/TimerQueue.h ./include/TimerId.h

Timestamp.o: ./src/Timestamp.cpp ./include/Timestamp.h

RedisClient.o: ./src/RedisClient.cpp ./include/RedisClient.h


clean: 
	rm *.o
