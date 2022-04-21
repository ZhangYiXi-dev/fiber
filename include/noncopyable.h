#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace zyx
{
    //对象无法拷贝与构造
	class Noncopyable 
	{
	public:
        Noncopyable() {};
        /**
         * @brief 拷贝构造函数(禁用)
         */
        Noncopyable(const Noncopyable&) = delete;

        /**
         * @brief 赋值函数(禁用)
         */
        Noncopyable& operator=(const Noncopyable&) = delete;
	};
}
#endif // NONCOPYABLE_H

