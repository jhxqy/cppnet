//
//  string_utils.hpp
//  cppnet
//
//  Created by 贾皓翔 on 2019/12/6.
//  Copyright © 2019 贾皓翔. All rights reserved.
//

#ifndef string_utils_hpp
#define string_utils_hpp

#include <cstring>

namespace cppnet{
namespace utils{
class StringUtils{
public:
    static void BZero(void *dest,size_t nbytes){
        memset(dest, 0, nbytes);
    }
    static bool BCmp(void *p1,void *p2,size_t nbytes){
        char *cp1=static_cast<char*>(p1);
        char *cp2=static_cast<char*>(p2);

        for(size_t i=0;i<nbytes;i++){
            if (cp1[i]!=cp2[i]) {
                return false;
            }
        }
        return true;
    }
    template<typename T>
    static void BZero(T &t){
         memset(&t, 0, sizeof(T));
    }
};
}
}

#endif /* string_utils_hpp */
