//
//  io_context.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/5.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef io_context_hpp
#define io_context_hpp
#include <unistd.h>

#include <stdio.h>
#include "Event.hpp"
#include <unordered_map>
namespace cppnet {
namespace async{

struct FdData{
    int count;
    bool count_close;
    
};
class SocketContext:public async::IoContext {
    std::unordered_map<int, FdData> fd_count_;
public:
    void FdCountInc(int fd){
        fd_count_[fd].count=fd_count_[fd].count+1;
    }
    bool TestFdCount(int fd){
        return fd_count_[fd].count!=0;
    }
    bool FdCountDec(int fd){
        fd_count_[fd].count=fd_count_[fd].count-1;
        if (fd_count_[fd].count<=0) {
            fd_count_[fd].count=0;
            return true&&!fd_count_[fd].count_close;
        }else{
            return false;
        }
    }
    bool FdCountClear(int fd){
        fd_count_[fd].count=0;
        return true;
    }
    
    ~SocketContext(){
        for(auto i:fd_count_){
            if(i.second.count!=0&&!i.second.count_close){
                close(i.first);
            }
        }
    }
    FdData& FdData(int fd){
        return fd_count_[fd];
    }
};
}

}
#endif /* io_context_hpp */
