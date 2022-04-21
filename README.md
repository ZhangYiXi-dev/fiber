# fiber
写在前面：这一份代码比较简单，只是使用了封装好的fiber与backtrace，但是前面两个发布的log与thread都包含在里面，可以自行加入使用
          接下来我也会继续封装一个协程调度，里面会把之前的日志模块，线程模块融合起来
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
//只是用了common.*与fiber.*，有兴趣可以把log加入，log在我之前的代码中有发布用法
1.common:将backtrace与assert封装
2.fiber：实现了协程的封装
```

**3.调用**

```cpp
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
```
