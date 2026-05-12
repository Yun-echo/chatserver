/*
muduo网络库给用户提供了两个主要的类
TcpServer：用于编写服务器程序的
TcpClient：用于编写客户端程序的

无论用什么库，哪怕是自己去设计使用epoll+线程池，这些网络库无非是将这些模型封装起来了
好处是能够把网络I/O的代码和业务代码区分开，让用户使用的时候可以将精力放在业务代码开发上，至于网络部分的代码由网络库直接帮忙封装完了
对于业务感兴趣的，用过libevent，或者其他libev等网络库的时候，会发现它们和muduo业务相似的地方是主要有两个暴露：
1.用户的连接和断开    2.用户的可读写事件
即只需要关注这两件事怎么做就行了，至于什么时候发生这件事情，由网络库上报，如何监听这些事情的发生，也是由网络库内部实现好的，不需要关心
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>    //绑定器
#include <string>
using namespace std;
using namespace muduo;       //using namespace 是 C++ 的“作用域别名”机制，它把指定命名空间内的所有标识符“拉”到当前作用域，避免重复书写冗长前缀
using namespace muduo::net;  //导入整个的命名空间 
using namespace placeholders;


/*基于muduo网络库开发服务器程序
第一步：组合TcpServer对象
第二步：创建EventLoop事件循环对象的指针
第三步：明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
    TcpServer的构造函数，没有默认构造，只有一个构造四个参数：
    1.EventLoop* loop 事件循环
    2.const InetAddress& listenAddr  muduo库封装好的可以去绑定ip地址和端口号
    3.const string& nameArg   可以给TcpServer一个名字
    4.Tcp协议的一个选项，不用关注
    因此需要三个参数，因为_server这个对象是需要TcpServer的构造的，如果不指定构造，就需要默认构造，但是没有默认构造，如果不指定的话，这个类就无法创建对象
第四步：在当前服务器类的构造函数当中，注册处理器连接的回调函数，和处理读写事件的回调函数
第五步：设置合适的服务端线程数量，muduo库会自己分配I/O线程和worker线程
    */


class ChatServer
{
public:
    ChatServer(EventLoop* loop,   //事件循环，reactor反应堆                  
            const InetAddress& listenAddr,   //ip+port
            const string& nameArg)           //服务器的名字   就是给这个线程绑定一个名字
        :_server(loop,listenAddr,nameArg)  ,_loop(loop) //我们所做的事情就是给loop,listenAddr,nameArg 以及 把形参传进来的loop事件循环保存起来，这样就可以在内部操作epoll了
    {
        //给服务器注册用户连接的创建和断开回调
        //给底层注册了一个onConnection方法，当底层监听到有用户的创建、连接和断开的时候就会帮我们调用onConnection函数，我们在这个函数里关注用户的创建断开就行
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));     //这个Callback都是没有返回值，一个形参变量的方法，
        //现在写的是一个成员方法，因为想访问下面的成员变量，写成员方法的时候会有一个this指针，就会和Callback的类型不同，因此，使用绑定器bind()，
        //人家的onConnection方法只有一个参数，而我们写的有两个参数，还有一个this ，所以要绑定this对象到这个方法中，_1参数占位符（人家的方法有一个参数）                                                             


        //给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        //给回调函数的参数占位符，告诉他我有几个参数，在调用的时候就给我传几个参数

        //设置服务器端的线程数量（默认有一个线程是I/O线程，只监听新用户的连接，工作线程是剩下的其余3个线程，但是已连接用户的断开事件会调用onConnection函数）
        _server.setThreadNum(2);    //一般4核设置4个线程
    }
    //开启事件循环
    void start() {
        _server.start();
    }
private:
    //专门处理用户的连接创建和断开；
    //我们自己在编写epoll的时候，从epoll拿过来一个事件以后，发现是listenfd的话，就会从listenfd 上 accept（相当于有新用户连接，拿出来一个专门和新用户通信的socket）
    //这一切。底层的muduo库都封装了，我们只需要暴露一个回调的接口onConnection
    void onConnection(const TcpConnectionPtr &conn)  //只管写就可以，不用管什么时候调用，已经将这个接口注册到muduo库上了，当有新用户的连接创建和原来用户的断开时，这个方法就会响应了
    {
        if(conn->connected()) {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "state:online" << endl;//peerAddress对端的  toIpPort()封装了ip地址和端口号
        }  
        else
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "state:offline" << endl;
            conn->shutdown();//连接断开以后，还可以执行这个语句 shutdown() 相当于 close(fd)  连接断开以后，就把服务端fd资源就回收掉了
           //_loop->quit();//如果不想再给用户提供服务了，可以用保存的epoll就是这个事件循环_loop调用quit()方法，相当于退出epoll了，整个服务器就结束了
        }    
    }  
    
    //专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn,   //连接 通过连接可以发送 读写数据
                            Buffer *buffer,          //缓冲区 提高数据收发性能
                            Timestamp time)        //接收到数据的时间信息; 时间戳Timestamp这个类里面有方法可以将time里的时间信息转成字符串
    {
        string buf = buffer->retrieveAllAsString();  //Buffer是muduo库里面封装的缓冲区的类，retrieveAllAsString()可以将Buffer *buffer接收的数据，全部放到字符串string buf中
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        conn->send(buf);  //一般接收到用户的信息，进行解码、处理、处理完成以后返回(收到什么返回什么 echo 一个回声服务器)
    }
    //第一步：定义一个_server
    TcpServer _server;  //最开始定义TcpServer时没有，是因为muduo中有很多的空间作用域 muduo::net::TcpServer，如果觉得麻烦可以using namespace muduo::net;
    //第二步
    EventLoop *_loop;   //组合EventLoop对象，看作epoll事件循环，可以向loop上注册感兴趣的事情/事件，如果有相应的事件发生，loop会给我们上报

};


int main()
{
    //main函数内是使用时候的代码，需要改的就是类的名称ChatServer，以及IP、端口号
    //首先要创建Chatserver这个对象，需要三个参数
    EventLoop loop;  //类似创建一个epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");   //事件循环的地址，addr是通过&listenAddr这个参数引用接收的可以直接传对象addr， _2 服务器的名称
    
    server.start();  //启动服务：将我们的 listenfd 通过 epoll_ctl 添加到 epoll 上，添加上之后就可以启动epoll_wait等待新用户的连接
    loop.loop();  //相当于调用了epoll_wait 以阻塞的方式等待新用户连接/已连接用户的读写事件等，如果这两种事件发生的化会帮我们回调两个on方法
    
    return 0;
}