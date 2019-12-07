//
//  buffer.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/3.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef buffer_hpp
#define buffer_hpp

#include <stdio.h>
#include <string>
namespace cppnet {
namespace buffer{
template<typename BufferType>
class BufferIterator{
    using SizeType=typename BufferType::SizeType;
    using DataType=typename BufferType::DataType;

    BufferType& buf_;
    SizeType pos_;
public:
    BufferIterator(BufferType& buf,SizeType pos):buf_(buf),pos_(pos){
    }
    SizeType& operator->(){
        return buf_[pos_];
    }
    bool operator==(const BufferIterator &b)const {
        return b.buf_==buf_&&b.pos_==pos_;
    }
    BufferIterator& operator+=(SizeType size)const{
        pos_+=size;
        return this;
    }
};
class MutableBuffer{
    char *data_;
    size_t size_;
public:
    using DataType=char;
    using SizeType=size_t;
    MutableBuffer():data_(nullptr),size_(0){
    }
    MutableBuffer(char *data,size_t size):data_(data),size_(size){
    }
    char * Data()const{
        return  data_;
    }
    size_t Size()const{
        return size_;
    }
    BufferIterator<MutableBuffer> Begin(){
        return BufferIterator<MutableBuffer>(*this,0);
    }
    BufferIterator<MutableBuffer> End(){
           return BufferIterator<MutableBuffer>(*this,size_);
    }
};
class ConstBuffer{
    const char* data_;
    size_t size_;
public:
    using DataType=char;
    using SizeType=size_t;
    ConstBuffer():data_(nullptr),size_(0){
    }
    ConstBuffer(const char *data,size_t size):data_(data),size_(size){
    }
    const char * Data()const{
        return data_;
    }
    size_t Size()const{
        return size_;
    }
};


inline ConstBuffer Buffer(const std::string &s){
    return ConstBuffer(s.c_str(),s.size());
}

}

}

#endif /* buffer_hpp */
