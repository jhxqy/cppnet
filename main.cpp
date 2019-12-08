//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include "stream_socket.hpp"
#include "dgram_socket.hpp"
using namespace std;
using namespace cppnet;
class session
  : public std::enable_shared_from_this<session>
{
public:
  session(socket::TcpSocket socket)
    : socket_(socket)
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
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

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    socket_.AsyncWriteSome(buffer::MutableBuffer(data_, length),
        [this, self](std::size_t /*length*/, error::IOError ec)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  socket::TcpSocket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

class server
{
public:
  server(async::SocketContext& io_context, short port)
    : acceptor_(io_context)
  {
      acceptor_.Bind(address::EndPoint(address::IPAddress::Parse("127.0.0.1"),port));
      acceptor_.Listen(5);
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.AsyncAccept(
        [this](socket::TcpSocket socket,error::IOError ec)
        {
          if (!ec)
          {
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
        WaitExit();
    }
    
};


class UdpServer{
    socket::UdpSocket sock;
    ssize_t waitSend;
    char buf[1024];
    void R(){
        sock.AsyncReceiveFrom(buffer::MutableBuffer(buf,1024), [this](ssize_t s,address::EndPoint e,error::IOError ec){
            if (ec) {
                cout<<ec.what()<<endl;
            }else{
                buf[s]=0;
                waitSend=s;
                cout<<buf;
                W(e);
                R();
            }
        });
    }
    void W(address::EndPoint ep){
        sock.AsyncSendTo(buffer::MutableBuffer(buf,waitSend),ep,[this](ssize_t s,error::IOError ec){
        });
    }
public:
    UdpServer(async::SocketContext &ctx):sock(ctx){
        sock.Bind(address::EndPoint(address::IPAddress::Parse("127.0.0.1"),8080));
        R();
    }
};

int main(int argc, char* argv[])
{
//try{
//
//    async::SocketContext io_context;
//    server s(io_context,8080);
//    ExitService e(io_context,fileno(stdin));
//    io_context.Run();
//  }
//  catch (std::exception& e)
//  {
//    std::cerr << "Exception: " << e.what() << "\n";
//  }
    async::SocketContext ctx;
    UdpServer us(ctx);
    ctx.Run();

  return 0;
}
