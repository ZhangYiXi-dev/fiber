#include "fiber.h"

namespace zyx
{
    static std::atomic<uint64_t> s_fiber_id {0};//协程id 
    static std::atomic<uint64_t> s_fiber_count {0}; //协程数

    static thread_local Fiber* t_fiber = nullptr; //保存当前协程
    static thread_local Fiber::ptr t_threadFiber = nullptr; //保存当前主协程
    //主协程初始化为EXEC
    Fiber::Fiber() 
    {
        m_state = EXEC;
        SetThis(this);

        if(getcontext(&m_ctx)) 
        {
            ZYX_ASSERT(false, "getcontext");
        }
        ++s_fiber_count;
    }
    //子协程构造，其link初始化为主协程，这样执行完后会回到主协程，use_caller现在为保留参数，未使用
    Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool use_caller)
    {
        m_id=++s_fiber_id;
        ++s_fiber_count;
        m_cb=cb;
        m_stacksize = stacksize ? stacksize : 1024*1024; 
        m_stack = malloc(m_stacksize);
        if(getcontext(&m_ctx)) 
        {
            ZYX_ASSERT(false,"getcontext");
        }
        m_ctx.uc_link = &(t_threadFiber->m_ctx);
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;
        if(!use_caller) 
        {
            makecontext(&m_ctx, &Fiber::MainFunc, 0);
        } 
        else
        {
            //makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
        }
    }

     Fiber::~Fiber()
     {
        --s_fiber_count;
         if(m_stack)
         {
            free(m_stack);
         }
         else
         {
             //为主协程
         }
     }
    
    //重置协程，即原始协程结束后，不释放栈空间，将其栈空间重新分配上下文
    void Fiber::reset(std::function<void()> cb)
    {
        ZYX_ASSERT(m_stack,"reset 栈不存在");
        ZYX_ASSERT(m_state==TERM||m_state==INIT || m_state == EXCEPT,"reset 栈不可重置");
        m_cb=cb;
        if(getcontext(&m_ctx))
        {
            ZYX_ASSERT(false,"getcontext");
        }
        m_ctx.uc_link=&(t_threadFiber->m_ctx);
        m_ctx.uc_stack.ss_sp = m_stack;
        m_ctx.uc_stack.ss_size = m_stacksize;
        makecontext(&m_ctx,&Fiber::MainFunc, 0);
        m_state = INIT;
    }

    //切换到当前协程运行
    void Fiber::swapIn()
    {
        ZYX_ASSERT(m_state != EXEC&&m_state != TERM,"swap in error"); //本身在执行状态或者结束状态，则无法切换
        SetThis(this); 
        m_state=EXEC;
        if(swapcontext(&(t_threadFiber->m_ctx),&m_ctx))
        {
            ZYX_ASSERT(false,"swapcontext error");
        }
    }
    //将当前协程切到后台
    void Fiber::swapOut()
    {
        //ZYX_ASSERT(m_state == EXEC,"swap out error");
        SetThis(t_threadFiber.get());
        if(swapcontext(&m_ctx,&(t_threadFiber->m_ctx)))
        {
            ZYX_ASSERT(false,"swapcontext error");
        }
    }
    //将当前协程设置为f
    void Fiber::SetThis(Fiber* f)
    {
        t_fiber=f;
    }

    //获取现在运行的协程指针
    //如果没有，说明主协程未初始化，则要初始化主协程
    //所以代码运行第一步要用此函数创建主协程
    Fiber::ptr Fiber:: GetThis()
    {
        if(t_fiber)
        {
            return t_fiber->shared_from_this();
        }
        else
        {
            Fiber::ptr main_fiber(new Fiber);
            ZYX_ASSERT(t_fiber == main_fiber.get(),"main_fiber create error");
            t_threadFiber = main_fiber;
            return t_fiber->shared_from_this();
        }
    }

    //主协程回调函数，该函数结束后会reset协程
    void Fiber::MainFunc()
    {
        Fiber::ptr cur = GetThis();
        ZYX_ASSERT(cur,"mainfunc GetThis");

        try
        {
            cur->m_cb();
            cur->m_state = TERM;
        }
        catch(const std::exception& e)
        {
            cur->m_state = EXCEPT;
            ZYX_ASSERT(false,"try");
        }
        cur->reset(cur->m_cb);
    }

    //将当前协程切换到后台,并设置为READY状态
    void Fiber::YieldToReady()
    {
        Fiber::ptr cur = GetThis();
        ZYX_ASSERT(cur->m_state == EXEC,"YieldToReady error");
        cur->m_state = READY;
        cur->swapOut();
    }

    //将当前协程切换到后台,并设置为HOLD状态
    void Fiber::YieldToHold()
    {
        Fiber::ptr cur = GetThis();
        ZYX_ASSERT(cur->m_state == EXEC,"YieldToReady error");
        cur->m_state = HOLD;
        cur->swapOut();
    }

    //返回当前协程的总数量
    uint64_t Fiber::TotalFibers()
    {
        return s_fiber_count;
    }
    
}