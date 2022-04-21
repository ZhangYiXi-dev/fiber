#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <execinfo.h>
#include <vector>
#include <sstream>
#include <cxxabi.h> 

//断言宏封装
#define ZYX_ASSERT(x,str) \
         if(!(x)){\
            std::cout<<zyx::BacktraceToString(3,str)<<std::endl;\
            assert(x);\
         }
namespace zyx
{
   void Backtrace(std::vector<std::string>& bt,int size);
   std::string BacktraceToString(int size,const std::string str);
   static std::string demangle(const char* str);
}
#endif
