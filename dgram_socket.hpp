//
//  dgram_socket.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/6.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef dgram_socket_hpp
#define dgram_socket_hpp

#include <stdio.h>
#include <string>
#include "buffer.hpp"
#include "socket_config.hpp"
#include "io_context.hpp"
#include "socket_config.hpp"
#include "end_point.hpp"
#include <sys/socket.h>

#include "error.hpp"
namespace cppnet{
namespace socket{
class DgramSocket:public SocketConfig<Udp>{
public:
    DgramSocket(async::SocketContext &ctx):SocketConfig<Udp>(ctx){
        
    }
    template<typename Buffer>
    ssize_t SendTo(const Buffer &buf,const address::EndPoint &ep ){
        return ::sendto(NativeHandle(), buf.Data(), buf.Size(), 0, ep.GetSockAddr(), sizeof(ep.GetSockAddr()));
    }
    template<typename Buffer>
    ssize_t ReceiveFrom(const Buffer &buf,address::EndPoint &ep){
        address::EndPoint::SockAddrType addr;
        socklen_t socklen=sizeof(addr);
        ssize_t len=::recvfrom(NativeHandle(), buf.Data(), buf.Size(), 0, (sockaddr*)&addr, &socklen);
        ep.SetSockAddr(addr);
        return len;
    }
    template<typename Buffer,typename Handler>
    void AsyncSendTo(const Buffer &buf,const address::EndPoint &ep ,Handler func){
        auto fd=NativeHandle();
        GetContext().AddEvent(new async::EventBase(NativeHandle(), async::EventBaseType::write, [fd,buf,func,ep](int){
            error::IOError ec;
            address::EndPoint::SockAddrType sock=ep.GetSockAddr();
            ssize_t sendto_result=::sendto(fd, buf.Data(), buf.Size(), 0, (sockaddr*)&sock, sizeof(sock));
            if(sendto_result<=0){
                ec.SetValue(true);
                ec.SetMessage(strerror(errno));
            }
            func(sendto_result,ec);
        }));
    }
    template<typename Buffer,typename Handler>
    void AsyncReceiveFrom(const Buffer &buf,Handler func){
        auto fd=NativeHandle();
        GetContext().AddEvent(new async::EventBase(NativeHandle(),async::EventBaseType::read,[fd,buf,func](int){
            error::IOError ec;
            address::EndPoint::SockAddrType addr;
            address::EndPoint::SockAddrSizeType alen=sizeof(addr);
            
            ssize_t recvfrom_result=::recvfrom(fd, buf.Data(), buf.Size(), 0, (sockaddr*)&addr, &alen);
            if(recvfrom_result<=0){
                ec.SetValue(true);
                ec.SetMessage(strerror(errno));
            }
            address::EndPoint ep;
            ep.SetSockAddr(addr);
            func(recvfrom_result,ep,ec);
        }));
    }
    
};

class UdpSocket:public DgramSocket{
public:
    UdpSocket(async::SocketContext &ctx):DgramSocket(ctx){
        int fd=::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        SetNativeHandle(fd);

    }
    UdpSocket(async::SocketContext &ctx,DgramSocket::NativeHandleType fd):DgramSocket(ctx){
        SetNativeHandle(fd);
    }
    void Bind(const address::EndPoint &ep){
        address::EndPoint::SockAddrType val=ep.GetSockAddr();
        if(bind(NativeHandle(),(sockaddr*)(&val),sizeof(val))==0){
        }else{
            throw exception::BindException(strerror(errno));
        }
    }
    
    
};



}

}
#endif /* dgram_socket_hpp */
