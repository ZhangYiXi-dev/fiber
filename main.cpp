/***
 * 包装协程并实现调度
 * 注：此代码的协程调度方式为一个主协程调度剩下的所有协程，
 *     也就是只能从主协程切到各个子协程和从各个子协程切回主协程，
 *     没有实现各个子协程间随意切换
*/
#include <iostream>
#include <vector>
#include <time.h>
#include <assert.h>
//#include "zyx_thread.h"
//#include "measure_time.h"
#include "common.h"
#include "fiber.h"

void fun_cb()
{
    std::cout<<"start fun_cb"<<std::endl;
    zyx::Fiber::ptr f=zyx::Fiber:: GetThis(); //获取当前协程
    f->YieldToHold();//切换回主协程
    std::cout<<"end fun_cb"<<std::endl;
}
int main()
{
    std::cout<<"start main"<<std::endl;
    zyx::Fiber::GetThis(); //创建主协程，即该上下文
    zyx::Fiber::ptr f(new zyx::Fiber(fun_cb));//创建子协程
    f->swapIn();//从主协程切换到子协程
    std::cout<<"end main"<<std::endl;
    f->swapIn();//从主协程切换到子协程
    std::cout<<"end"<<std::endl; 
     f->swapIn();//从主协程切换到子协程
}