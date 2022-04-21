#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

namespace zyx
{
    //�����޷������빹��
	class Noncopyable 
	{
	public:
        Noncopyable() {};
        /**
         * @brief �������캯��(����)
         */
        Noncopyable(const Noncopyable&) = delete;

        /**
         * @brief ��ֵ����(����)
         */
        Noncopyable& operator=(const Noncopyable&) = delete;
	};
}
#endif // NONCOPYABLE_H

