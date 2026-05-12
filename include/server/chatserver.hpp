#ifndef CHATSERVER_H    //如果宏 CHATSERVER_H ‌未被定义‌，则继续执行后续代码
#define CHATSERVER_H    //定义该宏，标记此文件已被包含

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;

//聊天服务器的主类
class ChatServer
{
public:
    //初始化聊天服务器对象
    ChatServer(EventLoop *loop,     //构造函数为了构造成员变量
        const InetAddress &listenAddr,
        const string &nameArg);

    //启动服务
    void start();
private:
    //注册两个方法
    //给TcpServer注册一个新用户的连接、连接断开以及已连接用户的可读写事件，响应连接、消息相关的事件
    //上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr &);

    //上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr &,
                            Buffer *,
                            Timestamp);
    TcpServer _server;    //组合的muduo库，实现服务器功能的类对象；TcpServer这个类产生的对象，作为一个组合对象
    EventLoop *_loop;     //指向事件循环对象的指针，可以在合适的时候调用quit 来退出这个事件循环

};

#endif    //结束条件编译块