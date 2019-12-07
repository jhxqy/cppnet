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
namespace cppnet{

class DgramSocket{
    async::SocketContext &ctx_;
public:
    DgramSocket(async::SocketContext &ctx):ctx_(ctx){
        
    }
    async::SocketContext& GetContext(){
        return ctx_;
    }
    template<typename Buffer>
    ssize_t SendTo(const Buffer &buf);
    template<typename Buffer>
    ssize_t ReceiveFrom(const Buffer &buf);
    template<typename Buffer,typename Handler>
    void AsyncSendTo(const Buffer &buf,Handler func);
    template<typename Buffer,typename Handler>
    void AsyncReceiveFrom(const Buffer &buf,Handler func);
    
    
    
};
}
#endif /* dgram_socket_hpp */
