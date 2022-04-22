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
#include "zyx_thread.h"
#include "measure_time.h"
#include "common.h"
#include "log.h"
#include "fiber.h"
zyx::Logger::ptr log_main = (new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, true))->Getlogger(); 

void fun_cb()
{
    ZYX_LOG_DEBUG(log_main,"start fun_cb");
    zyx::Fiber::ptr f=zyx::Fiber:: GetThis(); //获取当前协程
    f->YieldToHold();//切换回主协程
    ZYX_LOG_DEBUG(log_main,"end fun_cb");
}
void thread_cb(void* arg)
{
    zyx::Fiber::GetThis(); //创建主协程，即该上下文
    zyx::Fiber::ptr f(new zyx::Fiber(fun_cb));//创建子协程
     ZYX_LOG_DEBUG(log_main,"start main");
    f->swapIn();//从主协程切换到子协程
    ZYX_LOG_DEBUG(log_main,"end main");
    f->swapIn();//从主协程切换到子协程
    ZYX_LOG_DEBUG(log_main,"end");

     //f->swapIn();//从主协程切换到子协程
}
int main()
{
    std::vector<zyx::Thread::ptr> thr;
    for(int i=0;i<20;i++)
    {
        zyx::Thread::ptr t(new zyx::Thread(thread_cb,"thread"+std::to_string(i)));
        thr.push_back(t);
    }
    for(auto th:thr)
    {
        th->join();
    }
}