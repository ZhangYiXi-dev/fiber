#ifndef FIBER_H
#define FIBER_H

#include <iostream>
#include <ucontext.h>
#include <memory>
#include <functional>
#include <atomic>
#include <assert.h>
#include "common.h"

namespace zyx
{
    class Fiber : public std::enable_shared_from_this<Fiber>
    {
    private:
        //主协程的构造
        Fiber();  
    public:
        typedef std::shared_ptr<Fiber> ptr;
        /**
        * @brief 协程状态
        */
        enum State {
            /// 初始化状态
            INIT,
            /// 暂停状态
            HOLD,
            /// 执行中状态
            EXEC,
            /// 结束状态
            TERM,
            /// 可执行状态
            READY,
            /// 异常状态
            EXCEPT
        };
        /**
         * @brief 构造函数
         * @param[in] cb 协程执行的函数
         * @param[in] stacksize 协程栈大小
         * @param[in] use_caller 是否在MainFiber上调度
         */
        Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
        //析构函数
        ~Fiber();

        /**
         * @brief 重置协程执行函数,并设置状态
         * @pre getState() 为 INIT, TERM, EXCEPT
         * @post getState() = INIT
         */
        void reset(std::function<void()> cb);

        /**
         * @brief 将当前协程从主协程切换到运行状态
         */
        void swapIn();

        /**
         * @brief 将当前协程切换到后台
         */
        void swapOut();

        //获取协程id
        uint64_t getId() const { return m_id;}
        //获取当前协程状态
        State getState() const { return m_state;}
        /**
         * @brief 协程执行函数
         * @post 执行完成返回到线程主协程,将子协程reset
         */
        static void MainFunc();

        /**
         * @brief 设置当前线程的运行协程
         * @param[in] f 运行协程
         */
        static void SetThis(Fiber* f);
        /**
         * @brief 返回当前所在的协程
         */
        static Fiber::ptr GetThis();

        /**
         * @brief 将当前协程切换到后台,并设置为READY状态
         * @post getState() = READY
         */
        static void YieldToReady();

        /**
         * @brief 将当前协程切换到后台,并设置为HOLD状态
         * @post getState() = HOLD
         */
        static void YieldToHold();

        /**
         * @brief 返回当前协程的总数量
         */
        static uint64_t TotalFibers();


    private:
        //协程id
        uint64_t m_id;
        //运行时栈大小
        uint32_t m_stacksize=0;
        // 协程运行栈指针
        void* m_stack = nullptr;
        //协程状态
        State m_state = INIT;
        //协程上下文
        ucontext_t m_ctx;
        // 协程运行函数
        std::function<void()> m_cb;
    };
}
#endif