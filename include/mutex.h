#ifndef MUTEX_H
#define MUTEX_H
#include "noncopyable.h"
#include <pthread.h>
#include <stdint.h>
#include <semaphore.h>
#include <memory>
namespace zyx
{
    /**
    * @brief 信号量
    */
   class Semaphore : Noncopyable
   {
    public:
        /**
         * @brief 构造函数
         * @param[in] count 信号量值的大小
         */
        Semaphore(uint32_t count = 0);
        //销毁信号量
        ~Semaphore();
        //获取信号量 信号量--
        void wait();
        //释放信号量 信号量++
        void post();
    private:
        sem_t m_semaphore;
   };
    /**
    * @brief 互斥量
    */
    class Mutex: Noncopyable
    {
    public:
        //初始化锁
        Mutex() { pthread_mutex_init(&m_mutex, nullptr);}
        //离开作用域后销毁锁
        ~Mutex(){ pthread_mutex_destroy(&m_mutex);}
        //加锁
        void lock(){ pthread_mutex_lock(&m_mutex);}
        //解锁
        void unlock() { pthread_mutex_unlock(&m_mutex);}
    private:
        pthread_mutex_t m_mutex;

    };

    /**
    * @brief 自旋锁
    */
    class Spinlock : Noncopyable 
    {
    public:
        //初始化自旋锁
        Spinlock() { pthread_spin_init(&m_mutex, 0);}
        //离开作用域后销毁锁
        ~Spinlock() { pthread_spin_destroy(&m_mutex);}
        void lock() { pthread_spin_lock(&m_mutex);}
        void unlock() { pthread_spin_unlock(&m_mutex);}
    private:
        /// 自旋锁
        pthread_spinlock_t m_mutex;
};

}
#endif