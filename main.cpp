//
//  socket_config.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/4.
//  Copyright © 2019 贾皓翔. All rights reserved.
//


#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include "cppnet.hpp"
using namespace std;
using namespace cppnet;
class session: public std::enable_shared_from_this<session>{
public:
    
    session(socket::TcpSocket socket): socket_(socket){
    
}
    void start(){
        do_read();
    }

private:
void do_read(){
    auto self(shared_from_this());
    socket_.AsyncReadSome(buffer::MutableBuffer(data_, max_length),
        [this, self](std::size_t length, error::IOError ec)
        {
          if (!ec)
          {
            
            do_write(length);
          }else{
              cout<<"close"<<endl;
          }
        });
    
}

    void do_write(std::size_t length){
    auto self(shared_from_this());
    socket_.AsyncWriteSome(buffer::MutableBuffer(data_, length),[this, self](std::size_t /*length*/, error::IOError ec){
          if (!ec){
            do_read();
          }
        });
  }

  socket::TcpSocket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class TcpServer{
public:
    TcpServer(async::SocketContext& io_context, short port): acceptor_(io_context){
        acceptor_.ReUseAddr(true);
        acceptor_.ReUsePort(true);

        acceptor_.Bind(address::EndPoint(address::IPAddress::Parse("127.0.0.1"),port));
        acceptor_.Listen(5);
        do_accept();
    }

private:
    void do_accept(){
        acceptor_.AsyncAccept([this](socket::TcpSocket socket,error::IOError ec){
            if (!ec){
                cout<<socket.GetRemoteEndPoint().ToString()<< " connected!"<<endl;
                std::make_shared<session>(socket)->start();
                
            }

            do_accept();
        });
    }

    socket::TcpSocket acceptor_;
};

char buf[1024];

class ExitService{
    socket::TcpSocket socket_;
    char buff[1024];
    void WaitExit(){
        socket_.AsyncReadSome(buffer::MutableBuffer(buff,1024),[this](size_t size,error::IOError e){
            buff[size]=0;
            if (strcmp(buff, "exit\n")==0) {
                exit(0);
            }else{
                cout<<buff;
                cout<<"命令不存在！"<<endl;
                WaitExit();
            }
        });
        
    }
public:
    ExitService(async:: SocketContext &ctx,int fd):socket_(ctx,fd){
        socket_.SetCountClose(true);
        WaitExit();
    }
    
};
class UdpServer{
public:
    UdpServer(async::SocketContext & io_context, short port)
            : socket_(io_context){
        socket_.ReUseAddr(true);
        socket_.ReUsePort(true);

        socket_.Bind(address::EndPoint(address::EndPoint(address::IPAddress::Parse("127.0.0.1"),port)));
        do_receive();
        
    }

    void do_receive(){
        socket_.AsyncReceiveFrom(
                buffer::MutableBuffer(data_, max_length), sender_endpoint_,
                [this](ssize_t bytes_recvd,error::IOError ec)
                {
                    if (!ec && bytes_recvd > 0)
                    {
                        do_send(bytes_recvd);
                    }
                    else
                    {
                        do_receive();
                    }
                });
    }

    void do_send(std::size_t length){
        socket_.AsyncSendTo(
                buffer::MutableBuffer(data_, length), sender_endpoint_,
                [this](ssize_t bytes_recvd,error::IOError ec /*bytes_sent*/)
                {
                    do_receive();
                });
    }

private:
    socket::UdpSocket socket_;
    address::EndPoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};



int main(int argc, char* argv[])
{
    async::SocketContext io_context;
    TcpServer s1(io_context, 8080);
    ExitService e(io_context,fileno(stdin));
    UdpServer s(io_context,8081);

    io_context.Run();

    return 0;
}

