#include "zyx_thread.h"
#include <sys/types.h>

zyx::Logger::ptr log1 = (new zyx::LoggerManager(zyx::LogLevel::Level::DEBUG, true, true))->Getlogger(); //Ĭ��д��־��ʽ
namespace zyx
{
	static thread_local Thread* t_thread = nullptr;
	static thread_local std::string t_thread_name = "UNKNOW";

	Thread* Thread::GetThis() 
	{
    	return t_thread;
	}

	const std::string& Thread::GetName() 
	{
		return t_thread_name;
	}

	Thread::Thread(std::function<void(void*)> cb, const std::string& name,void* para)
	{
		m_cb = cb;
		m_name = name;
		param=para;
		int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
		if (rt)
		{
			ZYX_LOG_ERROR(log1,"pthread create error!");
		}
	}

	Thread::~Thread()
	{
		if(m_thread)
		{
		pthread_detach(m_thread);
		}
	}

	void Thread::join() {
		if (m_thread) {
			int rt = pthread_join(m_thread, nullptr);
			if (rt) {
				ZYX_LOG_ERROR(log1, "pthread_join thread fail");
				throw std::logic_error("pthread_join error");
			}
			m_thread=0;
		}
	}

	void* Thread::run(void* arg) {
		Thread* thread = (Thread*)arg;
		t_thread = thread;
		t_thread_name = thread->m_name;
		thread->m_id = syscall(SYS_gettid);
		pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

		thread->m_cb(thread->param);
		return 0;
	}
}