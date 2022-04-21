#ifndef MEASURE_TIME_H
#define MEASURE_TIME_H
#include <time.h>
#include <stdio.h>
namespace zyx
{
    class Measure_time
    {
        public:
        Measure_time(){ start=clock();}
        ~Measure_time()
        { 
            end=clock();
            printf("totile time=%f\n",(float)(end-start)*1000/CLOCKS_PER_SEC);
        }
        private:
        clock_t start;
        clock_t end;
    };
}
#endif