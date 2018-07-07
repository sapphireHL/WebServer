# WebServer
Cpp style http server.

## Http服务器
实现了简单的用户注册登录，上传、下载、删除文件功能，基于redis数据库存储用户的信息。

服务器的架构是线程池+epoll。

## 安装方法
下载解压之后进入目录
`bash
$ make
$ make clean
`
生成server可执行文件就是成功了。
还需要安装redis，请移步[redis安装](http://www.redis.cn/download.html)

## 运行
首先运行redis-server，然后在目录下运行server。
