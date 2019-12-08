//
//  stream_socket.hpp
//  cppnet
//  
//  Created by 贾皓翔 on 2019/12/4.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef stream_socket_hpp
#define stream_socket_hpp
#include <unistd.h>
#include <stdio.h>
#include "buffer.hpp"
#include "socket_config.hpp"
#include "io_context.hpp"
#include "error.hpp"
#include <iostream>
#include <sys/socket.h>
#include "end_point.hpp"
namespace cppnet{
namespace socket{
class StreamSocket:public SocketConfig<Tcp>{
public:
    StreamSocket(async::SocketContext &ctx):SocketConfig<Tcp>(ctx){}
    
    template<typename Buffer>
    ssize_t ReadSome(const Buffer&);
    template<typename Buffer>
    ssize_t WriteSome(const  Buffer&);
    
    template<typename Buffer,typename WriteHandler>
    void AsyncWriteSome(const Buffer &buf, WriteHandler&& handle);
    template<typename Buffer,typename ReadHandler>
    void AsyncReadSome(const Buffer &buf,ReadHandler&& handle);
};


template<typename Buffer>
ssize_t StreamSocket::ReadSome(const Buffer& b){
    return read(SocketConfig<Tcp>::NativeHandle(),b.Data(),b.Size());
}

template<typename Buffer>
ssize_t StreamSocket::WriteSome(const Buffer &b){
    return write(SocketConfig<Tcp>::NativeHandle(),b.Data(), b.Size());
}

template<typename Buffer,typename ReadHandler>
void StreamSocket::AsyncReadSome(const Buffer &buf, ReadHandler &&handle){
    GetContext().AddEvent(new async::EventBase(SocketConfig<Tcp>::NativeHandle(), async::EventBaseType::read,[buf,handle](int fd){
        ssize_t len=read(fd,buf.Data(), buf.Size());
        error::IOError ioe;
        if(len<=0){
            len=0;
            ioe.SetMessage("读取失败！");
            ioe.SetValue(true);
        }
        handle(static_cast<size_t>(len),ioe);
    }));
}




template<typename Buffer,typename WriteHandler>
void StreamSocket::AsyncWriteSome(const Buffer &buf, WriteHandler &&handle){
    GetContext().AddEvent(new async::EventBase(SocketConfig<Tcp>::NativeHandle(), async::EventBaseType::write,[buf,handle](int fd){
        ssize_t len=write(fd,buf.Data(), buf.Size());
        error::IOError ioe;

        if(len<=0){
            ioe.SetMessage("写入失败！");
            ioe.SetValue(true);
            len=0;
        }
        handle(static_cast<size_t>(len),ioe);
    }));
}



class TcpSocket:public StreamSocket{
    using HandleType=typename SocketConfig<Tcp>::NativeHandleType;
public:

    TcpSocket(async::SocketContext &ctx):StreamSocket(ctx){
        int fd=::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        SetNativeHandle(fd);

    } 
    TcpSocket(async::SocketContext &ctx,StreamSocket::NativeHandleType fd):StreamSocket(ctx){
        SetNativeHandle(fd);
    }
    void Bind(const address::EndPoint &ep){
        address::EndPoint::SockAddrType val=ep.GetSockAddr();
        if(bind(NativeHandle(),(sockaddr*)(&val),sizeof(val))==0){
        }else{
            throw exception::BindException(strerror(errno));
        }
    }
    void Listen(int backlog){
        if (listen(SocketConfig<Tcp>::NativeHandle(), backlog)!=0) {
            throw exception::ListenException(strerror(errno));
        }
    }
    
    TcpSocket Accept(){
        sockaddr sock;
        socklen_t len=sizeof(sock);
        int newfd;
        while(true){
            newfd=accept(SocketConfig<Tcp>::NativeHandle(), (sockaddr*)(&sock), &len);
            if (newfd<0) {
                if (errno==EINTR||errno==ECONNABORTED) {
                    continue;
                }else{
                    throw exception::AcceptException(strerror(errno));
                }
            }else{
                break;
            }
        }
        return TcpSocket(StreamSocket::GetContext(),newfd);
    }
    template<typename AcceptHandler>
    void AsyncAccept(AcceptHandler handler){
        GetContext().AddEvent(new async::EventBase(SocketConfig<Tcp>::NativeHandle(), async::EventBaseType::read, [this,handler](int fd){
            SocketConfig<Tcp>::NativeHandleType client=accept(fd, nullptr, nullptr);
            error::IOError ioe;
            if(client<0){
                ioe.SetValue(true);
                ioe.SetMessage(strerror(errno));
            }
            handler(TcpSocket(this->GetContext(), client),ioe);
        }));
    }
    ~TcpSocket(){
        
        
    }
};


}
}


#endif /* stream_socket_hpp */
