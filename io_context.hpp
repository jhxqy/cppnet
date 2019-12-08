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
class SocketContext:public async::IoContext {
    std::unordered_map<void*,int> fd_map_;
    std::unordered_map<int, int> fd_count_;
public:

    void Register(void *p,int fd){
        fd_map_[p]=fd;
    }
    void Cannel(void *p){
        fd_map_.erase(p);
    }
    int GetHandle(void *p){
        return fd_map_[p];
    }
    void FdCountInc(int fd){
        fd_count_[fd]=fd_count_[fd]+1;
    }
    bool TestFdCount(int fd){
        return fd_count_[fd]!=0;
    }
    bool FdCountDec(int fd){
        fd_count_[fd]=fd_count_[fd]-1;
        if (fd_count_[fd]<=0) {
            fd_count_[fd]=0;
            return true;
        }else{
            return false;
        }
    }
    bool FdCountClear(int fd){
        fd_count_[fd]=0;
        return true;
    }
    
    ~SocketContext(){
        for(auto i:fd_count_){
            if(i.second!=0){
                close(i.first);
            }
        }
    }
};
}

}
#endif /* io_context_hpp */
