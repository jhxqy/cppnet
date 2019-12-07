//
//  main.cpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/3.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#include <iostream>
#include "stream_socket.hpp"
using namespace std;
using namespace cppnet;

char buf[1024];

void f(size_t size){
    if(size==0){
        cout<<"error!"<<endl;
        return ;
    }
    cout<<string(buf,size);
}
async::IoContext c;

void ReadF(int fd){
    char buf[1024];
    ssize_t n=read(fd, buf, 1024);
    buf[n]=0;
    cout<<"异步读入"<<buf;


}
class A{
    int a,b;
public:
    void* AThis(){
        return this;
    }
};

class B{
    int a,b;
public:
    void* BThis(){
        return this;
    }
};
class C:public A,public B{
    int a,b;
public:
    void* CThis(){
        return this;
    }
};


int main(int argc, const char * argv[]) {
    cppnet::async::SocketContext ctx;
    socket::TcpSocket tcp(ctx);
    tcp.ReUsePort(true);
    tcp.Bind(address::EndPoint(address::IPAddress::Parse("127.0.0.1"),8080));
    tcp.Listen(1);
    while (1) {
        auto client=tcp.Accept();
        auto s=client.WriteSome(buffer::Buffer("helloworld"));
        auto b=client;
        b.Close();
    }
    return 0;
}
