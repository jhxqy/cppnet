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
template<typename HandleType>
class StreamSocket{
    async::SocketContext *ctx_;
public:
    StreamSocket(async::SocketContext &s):ctx_(&s){}
    async::SocketContext& GetContext()const {
        return *ctx_;
    }
    void SetSocketContext(async::SocketContext &ctx){
        ctx_=&ctx;
    }
    template<typename Buffer>
    ssize_t ReadSome(const Buffer&);
    template<typename Buffer>
    ssize_t WriteSome(const  Buffer&);
    
    template<typename Buffer,typename WriteHandler>
    void AsyncWriteSome(const Buffer &buf, WriteHandler&& handle);
    template<typename Buffer,typename ReadHandler>
    void AsyncReadSome(const Buffer &buf,ReadHandler&& handle);
};


template<typename HandleType>
template<typename Buffer>
ssize_t StreamSocket<HandleType>::ReadSome(const Buffer& b){
    return read(ctx_->GetHandle(this),b.Data(),b.Size());
}

template<typename HandleType>
template<typename Buffer>
ssize_t StreamSocket<HandleType>::WriteSome(const Buffer &b){
    return write(ctx_->GetHandle(this),b.Data(), b.Size());
}

template<typename T>
class Test;

template<typename HandleType>
template<typename Buffer,typename ReadHandler>
void StreamSocket<HandleType>::AsyncReadSome(const Buffer &buf, ReadHandler &&handle){
    ctx_->AddEvent(new async::EventBase(ctx_->GetHandle(this), async::EventBaseType::read,[buf,handle](int fd){
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




template<typename HandleType>
template<typename Buffer,typename WriteHandler>
void StreamSocket<HandleType>::AsyncWriteSome(const Buffer &buf, WriteHandler &&handle){
    ctx_->AddEvent(new async::EventBase(ctx_->GetHandle(this), async::EventBaseType::write,[buf,handle](int fd){
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



class TcpSocket:public StreamSocket<int>,public SocketConfig<Tcp>{
    using HandleType=typename SocketConfig<Tcp>::NativeHandleType;
public:
    TcpSocket& operator=(const TcpSocket &s){
        this->SetSocketContext(s.GetContext());
        auto handle=GetContext().GetHandle((void*)(&s));
        if(GetContext().TestFdCount(GetContext().GetHandle(this))){
            GetContext().FdCountInc(GetContext().GetHandle(this));
        }

        SocketConfig<Tcp>::SetNativeHandle(handle);
        GetContext().Register(this, handle);
        GetContext().FdCountInc(handle);
        return *this;

    }
    TcpSocket(const TcpSocket&s):StreamSocket<int>(s.GetContext()){
        SocketConfig<Tcp>::NativeHandleType fd=s.GetContext().GetHandle((void*)(&s));
        SocketConfig<Tcp>::SetNativeHandle(fd);
        GetContext().Register(this, fd);
        GetContext().FdCountInc(GetContext().GetHandle(this));
    }
    TcpSocket(async::SocketContext &ctx,HandleType fd):StreamSocket<int>(ctx),SocketConfig<Tcp>(fd){
        ctx.Register(this, fd);
        GetContext().FdCountInc(GetContext().GetHandle(this));
    }
    TcpSocket(async::SocketContext &ctx):StreamSocket<int>(ctx){
        int fd=::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        SocketConfig<Tcp>::SetNativeHandle(fd);
        ctx.Register(this, fd);
        GetContext().FdCountInc(GetContext().GetHandle(this));
    }
    void Bind(const address::EndPoint &ep){
        address::EndPoint::SockAddrType val=ep.GetSockAddr();
        if(bind(StreamSocket<int>::GetContext().GetHandle(this),(sockaddr*)(&val),sizeof(val))==0){
        }else{
            throw exception::BindException(strerror(errno));
        }
    }
    void Listen(int backlog){
        if (listen(StreamSocket<int>::GetContext().GetHandle(this), backlog)!=0) {
            throw exception::ListenException(strerror(errno));
        }
    }
    
    TcpSocket Accept(){
        sockaddr sock;
        socklen_t len=sizeof(sock);
        int newfd;
        while(true){
            newfd=accept(StreamSocket<int>::GetContext().GetHandle(this), (sockaddr*)(&sock), &len);
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
        return TcpSocket(StreamSocket<int>::GetContext(),newfd);
    }
    template<typename AcceptHandler>
    void AsyncAccept(AcceptHandler handler){
        GetContext().AddEvent(new async::EventBase(StreamSocket<int>::GetContext().GetHandle(this), async::EventBaseType::read, [this,handler](int fd){
            SocketConfig<Tcp>::NativeHandleType client=accept(fd, nullptr, nullptr);
            error::IOError ioe;
            if(client<0){
                ioe.SetValue(true);
                ioe.SetMessage("accept error");
            }
            handler(TcpSocket(this->GetContext(), client),ioe);
        }));
    }
    
    
    
    void Close(){
        auto fd=GetContext().GetHandle(this);
        if(GetContext().TestFdCount(fd)){
            GetContext().FdCountClear(fd);
            close(GetContext().GetHandle(this));
            std::cout<<"结束了:"<<fd<<std::endl;
        }
       
        
    }
    ~TcpSocket(){
        auto fd=GetContext().GetHandle(this);
        if(GetContext().TestFdCount(fd)){
            if(GetContext().FdCountDec(GetContext().GetHandle(this))){
                close(GetContext().GetHandle(this));
                std::cout<<"结束了:"<<fd<<std::endl;
            }
        }
        
    }
};


}
}


#endif /* stream_socket_hpp */
