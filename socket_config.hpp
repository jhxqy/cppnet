//
//  socket_config.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/4.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef socket_config_hpp
#define socket_config_hpp

#include <stdio.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "io_context.hpp"
#include <iostream>

namespace cppnet {
namespace socket{
class Tcp{};
class Udp{};

template<typename Protocol>
class SocketConfig{
    async::SocketContext *ctx_;
public:
    using NativeHandleType=int;
    SocketConfig(async::SocketContext&ctx,NativeHandleType fd):fd_(fd),ctx_(&ctx){
        ctx_->FdCountInc(fd);
    }
    SocketConfig(async::SocketContext&ctx):ctx_(&ctx),fd_(-1){
        
    }
    SocketConfig(const SocketConfig &sock){
        this->ctx_=sock.ctx_;
        
        SetNativeHandle(sock.fd_);
    }
    SocketConfig& operator=(const SocketConfig&sock){
        this->ctx_=sock.ctx_;
        SetNativeHandle(sock.fd_);
        return *this;
    }
    ~SocketConfig(){
        if(ctx_->FdCountDec(fd_)){
            std::cout<<fd_<<" close"<<std::endl;
            close(fd_);
        }
    }
    
    void SetNativeHandle(NativeHandleType fd){
        if(fd<0){
            return ;
        }
        fd_=fd;
        ctx_->FdCountInc(fd_);
    }
    NativeHandleType NativeHandle()const{
        return fd_;
    }
    async::SocketContext& GetContext()const {
        return *ctx_;
    }
    
    
    
    void SetCountClose(bool val){
        ctx_->FdData(fd_).count_close=val;
    }
    
    void SetSocketContext(async::SocketContext &ctx){
        ctx_=&ctx;
    }
    
    
    
    
    
    
    
    
    // 通用套接字选项

    void Broadcast(bool);
    void OobInline(bool);
    void ReceiveBuf(size_t size);
    void SendBuf(size_t size);
    void SendOutTime(long ms);//ms
    void ReceiveOutTime(long ms);//ms
    void ReUseAddr(bool);
    void ReUsePort(bool);
    // IPV4套接字选项
    void TTL(int val);
    // fctl操作
    void NonBlock(bool);
    void SigDrive(bool);
    void SetOwn(int id);
private:
    NativeHandleType fd_;
};

template<typename Protocol>
void SocketConfig<Protocol>::Broadcast(bool val){
    int op=val?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_BROADCAST, &op, sizeof(op));
}
template<typename Protocol>
void SocketConfig<Protocol>::OobInline(bool val){
    int op=val?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_OOBINLINE, &op, sizeof(op));
}
template<typename Protocol>
void SocketConfig<Protocol>::ReceiveBuf(size_t size){
    setsockopt(fd_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}
template<typename Protocol>
void SocketConfig<Protocol>::SendBuf(size_t size){
    setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

template<typename Protocol>
void SocketConfig<Protocol>::SendOutTime( long ms){
    struct timeval tv;
    tv.tv_sec=ms/1000000; 
    tv.tv_usec=(ms%1000000)*1000;
    setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &tv, sizeof(tv));
}

template<typename Protocol>
void SocketConfig<Protocol>::ReceiveOutTime(long ms){
    struct timeval tv;
    tv.tv_sec=ms/1000000;
    tv.tv_usec=(ms%1000000)*1000;
    setsockopt(fd_, SOL_SOCKET, SO_SNDBUF, &tv, sizeof(tv));
}
template<typename Protocol>
void SocketConfig<Protocol>::ReUseAddr(bool val){
    int op=val?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_OOBINLINE, &op, sizeof(op));
}
template<typename Protocol>
void SocketConfig<Protocol>::ReUsePort(bool val){
    int op=val?1:0;
    setsockopt(fd_, SOL_SOCKET, SO_OOBINLINE, &op, sizeof(op));
}

template<typename Protocol>
void SocketConfig<Protocol>::TTL(int val){
    setsockopt(fd_,IPPROTO_IP,IP_TTL ,&val, sizeof(val));
}
template<typename Protocol>
void SocketConfig<Protocol>::NonBlock(bool val){
    int flags=fcntl(fd_,F_GETFL,0);
    if(val){
        flags|=O_NONBLOCK;
    }else{
        flags&=~O_NONBLOCK;
    }
    
    fcntl(fd_, F_SETFL,flags);
}
template<typename Protocol>
void SocketConfig<Protocol>::SigDrive(bool val){
    int flags=fcntl(fd_,F_GETFL,0);
    if(val){
        flags|=O_ASYNC;
    }else{
        flags&=~O_ASYNC;
    }
    fcntl(fd_, F_SETFL,flags);
}


}
}
#endif /* socket_config_hpp */
