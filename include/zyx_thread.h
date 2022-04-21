#ifndef ZYX_THREAD_H
#define ZYX_THREAD_H

#include <thread>
#include "log.h"
#include <functional>
#include <memory>
#include <pthread.h>
#include <semaphore.h>
#include <memory>
#include <stdint.h>
#include <atomic>
#include <list>
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "noncopyable.h"

namespace zyx {

	class Thread :public Noncopyable
	{
	public:
		typedef std::shared_ptr<Thread> ptr;
		/**
        * @brief 构造函数，初始化线程
        * @param[in] cb 回调函数 
		* @param[in] name 线程名称
		* @param[in] para 回调函数参数
        */
		Thread(std::function<void(void*)> cb, const std::string& name,void* para=nullptr);
		~Thread();
		//获取线程id
		pid_t getId() const { return m_id; }
		//获取线程名称
		const std::string& getName() const { return m_name; }
		//回收线程
		void join();
		//静态的获取当前线程指针，配合cpp文件中的静态线程范围内的变量使用
		static Thread* GetThis();
		//静态的获取当前线程名称，配合cpp文件中的静态线程范围内的变量使用
		static const std::string& GetName();
	private:
		//回调函数
		static void* run(void* arg);
	private:
		pid_t m_id;
		pthread_t m_thread ;
		std::string m_name;
		//自己的回调函数主体
		std::function<void(void*)> m_cb;
		//回调函数参数
		void* param;

	};
}
#endif 
