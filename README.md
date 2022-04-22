# fiber-log-thread
写在前面：这一份代码是将新加入的协程模块与之前的log，thread模块融合起来。实现了创建多个线程，每个线程中控制协程的demo。在多线程中输出还是要使用log，不然会出现输出内容错误(未加锁)
## 封装一个协程
**首先，此代码的协程调度方式为一个主协程调度剩下的所有协程，也就是只能从主协程切到各个子协程和从各个子协程切回主协程，没有实现各个子协程间随意切换**

**1.使用方法：**

```cpp
//已经写好makefile，执行以下语句即可
make
./fiber
```
**2.使用到的各个文件**

```cpp
1.common:将backtrace与assert封装
2.fiber：实现了协程的封装
3.log:写日志的功能（控制台与文件）
4.zyx_thread:线程功能
```

**3.调用**

```cpp
#include <iostream>
#include <vector>
#include <time.h>
#include <assert.h>
#include "zyx_thread.h"
#include "measure_time.h"
#include "common.h"
#include "log.h"
#include "fiber.h"
//默认输出格式："%D %t-%N %F %m%n"
//时间 线程id-线程名称 协程id  自定义信息
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
}
int main()
{
    std::vector<zyx::Thread::ptr> thr;
    for(int i=0;i<20;i++)
    {
        //创建线程
        zyx::Thread::ptr t(new zyx::Thread(thread_cb,"thread"+std::to_string(i)));
        thr.push_back(t);
    }
    for(auto th:thr)
    {
        //回收线程
        th->join();
    }
}
```
**4.输出：**
```cpp
//默认输出格式："%D %t-%N %F %m%n"
//时间 线程id-线程名称 协程id  自定义信息
```
![image](https://user-images.githubusercontent.com/83794882/164588710-4dbff747-2c8a-4c54-8993-6a53e4419294.png)

