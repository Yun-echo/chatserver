#include "json.hpp"
using json = nlohmann::json;   //给nlohmann::json命名一个更简单的名称 json

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

//json序列化示例1
void func1() {
    json js;                     //将js看作容器
    js["msg_type"] = 2;          //msg_type 是键,2 是值
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";
    
    string sendBuf = js.dump();       //dump输出

    cout<<sendBuf.c_str()<<endl;      //通过网络传的是char* ，用string对象的c_str()方法转为char*

}

//json序列化示例2
void func2() {
    json js;    //要使用序列化，先用json这个数据类型定义一个对象
    //添加数组
    js["id"] = {1,2,3,4,5};      //json中的键所对应的值可以是，int char string 也可以是 数组类型
    //添加key-value
    js["name"] = "zhang san";
    //添加对象
    js["msg"]["zhang san"] = "hello world";     //类似二维数组访问，相当于msg这个键所对应的值可以依然看作是一个json字符串
    js["msg"]["liu shuo"] = "hello china";      //在这个json字符串中又访问zhang san 这个键对应的值
    //上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    cout << js << endl;


}


//json序列化示例3
void func3() {
    json js;

    //直接序列化一个vector容器
    vector<int> vec;    //定义一个vector
    vec.push_back(1);   //给vector中添加了 1 2 5
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    //直接序列化一个map容器
    map<int, string> m;
    m.insert({1,"黄山"});
    m.insert({2,"华山"});
    m.insert({3,"泰山"});
    js["path"] = m;

    //想将数据发送出去，就采用这个方法
    string sendBuf = js.dump();   //将json数据对象  序列化成 json字符串

    cout<<sendBuf<<endl;          //进行发送


}


//json反序列化示例1
string func4() {             //返回字符串
    json js;                     //将js看作容器
    js["msg_type"] = 2;          //msg_type 是键,2 是值
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";
    
    string sendBuf = js.dump();       //dump输出

    //cout<<sendBuf.c_str()<<endl;      //通过网络传的是char* ，用string对象的c_str()方法转为char*

    return sendBuf;
}

//json反序列化示例2
string func5() {
    json js;    //要使用序列化，先用json这个数据类型定义一个对象
    //添加数组
    js["id"] = {1,2,3,4,5};      //json中的键所对应的值可以是，int char string 也可以是 数组类型
    //添加key-value
    js["name"] = "zhang san";
    //添加对象
    js["msg"]["zhang san"] = "hello world";     //类似二维数组访问，相当于msg这个键所对应的值可以依然看作是一个json字符串
    js["msg"]["liu shuo"] = "hello china";      //在这个json字符串中又访问zhang san 这个键对应的值
    //上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}};
    //cout << js << endl;
    return js.dump();


}


//json反序列化示例3
string func6() {
    json js;

    //直接序列化一个vector容器
    vector<int> vec;    //定义一个vector
    vec.push_back(1);   //给vector中添加了 1 2 5
    vec.push_back(2);
    vec.push_back(5);
    js["list"] = vec;

    //直接序列化一个map容器
    map<int, string> m;
    m.insert({1,"黄山"});
    m.insert({2,"华山"});
    m.insert({3,"泰山"});
    js["path"] = m;

    //想将数据发送出去，就采用这个方法
    string sendBuf = js.dump();   //将json数据对象  序列化成 json字符串

    //cout<<sendBuf<<endl;          //进行发送

    return sendBuf;


}


int main() {
    string recvBuf = func6();        //string recvBuf 是通过网络发送过来的，func4()是做序列化的
    //先进行数据的反序列化   json字符串 -> 反序列化为 数据对象（看作容器，方便直接访问）
    json jsbuf = json::parse(recvBuf);   //recvBuf从网络接收的字符串，返回一个json的对象，将json jsbuf看作一个容器
    //cout<<jsbuf["msg_type"]<<endl;
    //cout<<jsbuf["from"]<<endl;
    //cout<<jsbuf["to"]<<endl;
    //cout<<jsbuf["msg"]<<endl;

    //cout<<jsbuf["id"]<<endl;   //可以直接打印
    //auto arr = jsbuf["id"];    //也可以用数组下标打印
    //cout<<arr[2]<<endl;

    //auto msgjs = jsbuf["msg"];
    //cout<<msgjs["zhang san"]<<endl;
    //cout<<msgjs["liu shuo"]<<endl;

    vector<int> vec = jsbuf["list"];     // js对象里面的数组类型，直接放入vector容器中，让这个键对应的值是一个数组类型
    for(int &v : vec) {
        cout<<v<<" ";
    }
    cout<<endl;

    map<int, string> mymap = jsbuf["path"];
    for(auto &p : mymap) {
        cout<<p.first<<" "<<p.second<<endl;
    }
    cout<<endl;

    return 0;
}

