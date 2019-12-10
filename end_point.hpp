//
//  end_point.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/6.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef end_point_hpp
#define end_point_hpp

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "error.hpp"
#include <cerrno>
#include "string_utils.hpp"
namespace cppnet{
namespace address{
class EndPoint;
class IPAddress{
    /*
     int inet_aton(const char* *strptr,struct in_addr * addrptr);
     将一个字符串转换为in_addr 成功返回1，否则返回0
     
     char* inet_ntoa(struct in_addr inaddr);
     将一个in_addr转换为点分十进制指针
     */
    std::string ip_string_;
    in_addr addr_;
public:
    IPAddress(in_addr &addr,const std::string &ips):addr_(addr),ip_string_(ips){
        
    }
    //解析十进制点分字符串
    static IPAddress Parse(const std::string &str){
        in_addr in;
        if(inet_pton(AF_INET, str.c_str(), &in)){
            return IPAddress(in,std::string(str));
        }else{
            throw cppnet::exception::IpAddressParserException(strerror(errno));
        }
    }
    
    static IPAddress Any(){
        in_addr in;
        cppnet::utils::StringUtils::BZero(in); 
        return IPAddress(in,"");
    }
    operator std::string(){
         return ip_string_;
    }
    friend class EndPoint;
    
    
    
    
};

class EndPoint{
    
    sockaddr_in sock_;
public:
    using SockAddrType=sockaddr_in;
    using SockAddrSizeType=socklen_t;
    EndPoint(const IPAddress &ipa,short port){
        cppnet::utils::StringUtils::BZero(sock_);
        sock_.sin_family=AF_INET;
        sock_.sin_addr=ipa.addr_;
        sock_.sin_port=htons(port);
    }
    EndPoint(){
        
    }
    SockAddrType GetSockAddr()const {
        return sock_;
    }
    void SetSockAddr(const SockAddrType &addr){
        sock_=addr;
    }
    
    operator std::string()const { 
        std::string res;
        char buf[100];
        inet_ntop(AF_INET, &sock_, buf, sizeof(sock_));
        res=res+std::string(buf)+":"+std::to_string(ntohs(sock_.sin_port));
        return res;
    }
    std::string ToString()const{
        return std::string(*this);
    }
    
    
    
};
}
}
#endif /* end_point_hpp */
