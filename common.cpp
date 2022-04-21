#include "common.h"

namespace zyx
{
    static std::string demangle(const char* str) 
    {
        size_t size = 0;
        int status = 0;
        std::string rt;
        rt.resize(256);
        if(1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) {
            char* v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
            if(v) {
                std::string result(v);
                free(v);
                return result;
            }
        }
        if(1 == sscanf(str, "%255s", &rt[0])) {
            return rt;
        }
        return str;
    }
    void Backtrace(std::vector<std::string>& bt,int size)
    {
        void** array = (void**)malloc((sizeof(void*) * size));
        size_t s = backtrace(array, size);
        char** strings = backtrace_symbols(array, s);
        if(strings==NULL)
        {

        }
        for(int i=0;i<s;++i)
        {
            bt.push_back(demangle(strings[i]));
        }
        free(array);
        free(strings);
    }
    std::string BacktraceToString(int size,const std::string str)
    {
        std::vector<std::string> bt;
        Backtrace(bt, size);
        std::stringstream ss;
        for(size_t i = 0; i < bt.size(); ++i) 
        {
            ss << bt[i] <<" "<<str<< std::endl;
        }
        return ss.str();
    }
}