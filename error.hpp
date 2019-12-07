//
//  error.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/5.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef error_hpp
#define error_hpp

#include <stdio.h>
#include <string>
#include <stdexcept>
#include <stdio.h>
#define createException(name,base) class name##Exception:public base {\
public:\
    name##Exception(const std::string &message):base(message){\
    }\
};
namespace cppnet{
namespace exception{
createException(Net, std::runtime_error);

createException(IpAddressParser, NetException);
createException(Bind, NetException);
createException(Listen, NetException);
createException(Accept, NetException);

}



namespace error{
class Error{
public:
    virtual std::string what()=0;
    virtual void SetMessage(const std::string &s)=0;
    virtual void SetValue(bool)=0;
};

class IOError:public Error{
    bool val_;
    std::string message_;
public:
    IOError():val_(false){}
    IOError(const std::string &s):val_(false),message_(s){
        
    }
    virtual void SetValue(bool v){
        val_=v;
    }
    virtual operator bool(){
        return val_;
    }
    virtual std::string what(){
        return message_;
    }
    virtual void SetMessage(const std::string &s){
        message_=s;
    }
};

}
}
#endif /* error_hpp */
