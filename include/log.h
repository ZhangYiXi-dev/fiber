#ifndef LOG_H
#define LOG_H
#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <map>
#include<time.h>
#include "mutex.h"
#include "zyx_thread.h"
#include "fiber.h"
/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define ZYX_LOG_LEVEL_DEBUG(logger, level,str) \
        {\
        zyx::LogEvent::ptr eptr(new zyx::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, syscall(SYS_gettid),\
                zyx::Fiber::GetThis()->getId(), time(0), zyx::Thread::GetThis()->GetName()));\
                eptr->getSS()<<str;\
                logger->debug(eptr) ;}
        

#define ZYX_LOG_DEBUG(logger,str) ZYX_LOG_LEVEL_DEBUG(logger, zyx::LogLevel::DEBUG,str)
  
  /**
  * @brief 使用流式方式将日志级别info的日志写入到logger
  */
#define ZYX_LOG_LEVEL_INFO(logger, level,str) \
        {\
        zyx::LogEvent::ptr eptr(new zyx::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, syscall(SYS_gettid),\
                zyx::Fiber::GetThis()->getId(), time(0), zyx::Thread::GetThis()->GetName()));\
                eptr->getSS()<<str;\
                logger->info(eptr);}
        

#define ZYX_LOG_INFO(logger,str) ZYX_LOG_LEVEL_INFO(logger, zyx::LogLevel::INFO,str)

  /**
  * @brief 使用流式方式将日志级别warn的日志写入到logger
  */
#define ZYX_LOG_LEVEL_WARN(logger, level,str) \
        {\
       zyx::LogEvent::ptr eptr(new zyx::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, syscall(SYS_gettid),\
                zyx::Fiber::GetThis()->getId(), time(0), zyx::Thread::GetThis()->GetName()));\
                eptr->getSS()<<str;\
                logger->warn(eptr);}  

#define ZYX_LOG_WARN(logger,str) SYLAR_LOG_LEVEL(logger, zyx::LogLevel::WARN,str)

   /**
   * @brief 使用流式方式将日志级别error的日志写入到logger
   */
#define ZYX_LOG_LEVEL_ERROR(logger, level,str) \
        {\
        zyx::LogEvent::ptr eptr(new zyx::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, syscall(SYS_gettid),\
                zyx::Fiber::GetThis()->getId(), time(0), zyx::Thread::GetThis()->GetName()));\
                eptr->getSS()<<str;\
                logger->error(eptr);}  
#define ZYX_LOG_ERROR(logger,str) ZYX_LOG_LEVEL_ERROR(logger, zyx::LogLevel::ERROR,str)

    /**
     * @brief 使用流式方式将日志级别fatal的日志写入到logger
     */
#define ZYX_LOG_LEVEL_FATAL(logger, level,str) \
        {\
        zyx::LogEvent::ptr eptr(new zyx::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, syscall(SYS_gettid),\
                zyx::Fiber::GetThis()->getId(), time(0), zyx::Thread::GetThis()->GetName()));\
                eptr->getSS()<<str;\
                logger->error(eptr);}  
#define ZYX_LOG_FATAL(logger,str) ZYX_LOG_LEVEL_FATAL(logger, zyx::LogLevel::FATAL,str)

namespace zyx
{
class LogFormatter;
class Logger;

class LogLevel 
{
public:
    /**
    * @brief 日志级别枚举
    */
    enum Level {
        /// 未知级别
        UNKNOW = 0,
        /// DEBUG 级别
        DEBUG = 1,
        /// INFO 级别
        INFO = 2,
        /// WARN 级别
        WARN = 3,
        /// ERROR 级别
        ERROR = 4,
        /// FATAL 级别
        FATAL = 5
    };

    /**
        * @brief 将日志级别转成文本输出
        * @param[in] level 日志级别
        */
        static const char* ToString(LogLevel::Level level);

        /**
            * @brief 将文本转换成日志级别
            * @param[in] str 日志级别文本
            */
         static LogLevel::Level FromString(const std::string& str);
};

class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    //构造函数，通过一些系统函数，获得事件的内容
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
        , const char* m_file, int32_t m_line, uint32_t m_elapse, uint32_t m_threadId,
        uint32_t m_fiberId, uint64_t m_time, std::string m_threadName);

    //下述函数为获得其各个成员变量的值用于后期打印
    const char* getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string& getThreadName() const { return m_threadName; }
    const std::string& getFiberName() const { return m_FiberName; }
    std::string getContent() const { return m_ss.str(); }
    std::stringstream& getSS() { return m_ss; }

private:
    /// 文件名
    const char* m_file = nullptr;
    /// 行号
    int32_t m_line = 0;
    /// 程序启动开始到现在的毫秒数
    uint32_t m_elapse = 0;
    /// 线程ID
    uint32_t m_threadId = 0;
    /// 协程ID
    uint32_t m_fiberId = 0;
    /// 时间戳
    uint64_t m_time = 0;
    /// 线程名称
    std::string m_threadName;
    /// 协程名称
    std::string m_FiberName;
    /// 日志内容流(配合stringstream& getSS()用”<<”将自己要打印的错误信息传入)
    std::stringstream m_ss;
    /// 日志器
    std::shared_ptr<Logger> m_logger;
    /// 日志等级
    LogLevel::Level m_level;
};

class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(std::string formate="%D %t-%N %F %m%n");//时间 线程id-线程名称 协程id  自定义信息
    std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    //将模板格式进行解析，解析成FormatItem放入vector<FormatItem::ptr>中用于输出
     void init(); 
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        /**
         * @brief 析构函数
         */
        virtual ~FormatItem() {}
        /**
         * @brief 格式化日志到流
         * @param[in, out] os 日志输出流
         * @param[in] logger 日志器
         * @param[in] level 日志等级
         * @param[in] event 日志事件
         */
         //该虚函数在cpp中被重写，然后通过std::vector<FormatItem::ptr> m_items已不同形式被输出
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    
    /// 日志格式模板(可自定义，即在构造时设置)
    std::string m_pattern;
    /// 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
};

class LogAppender
{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    //输出
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
protected:
    LogFormatter::ptr m_formatter;
    Mutex mutex;
};

class Logger : public std::enable_shared_from_this<Logger>
{
public:
    typedef std::shared_ptr<Logger> ptr;
    //初始化logger的level与格式
    Logger(LogLevel::Level my_level, LogFormatter::ptr m_formatter);
    void log(LogLevel::Level level, LogEvent::ptr event);
    //不同级别下写输出
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);
    //添加输出位置(一个日志器可以同时把相同内容写入不同的地方)
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() { return my_level; }
private:
    LogLevel::Level my_level = zyx::LogLevel::DEBUG;
    LogEvent::ptr event;
    LogFormatter::ptr m_formatter;
    std::vector<LogAppender::ptr> m_appenders;
};

class StdoutLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    StdoutLogAppender(LogFormatter::ptr p) { m_formatter = p; }
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class FileLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string filename, LogFormatter::ptr p);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
private:
    // 文件路径
    std::string m_filename;
    // 文件流
    std::ofstream m_filestream;
    // 上次重新打开时间
    uint64_t m_lastTime = 0;
};

class LoggerManager
{
public:
    typedef std::shared_ptr<LoggerManager> ptr;
    //默认方式初始化，默认下会已默认formate向控制台与test文件中写输出信息
    LoggerManager(LogLevel::Level level= LogLevel::Level::DEBUG,bool write_std=true, bool write_file=true);
    //自定义初始化方式，即可以指定已什么格式向哪些文件和控制台输出信息
    LoggerManager(LogLevel::Level level, bool write_std, std::vector<std::string>write_file, std::string formate);
    void add_file(std::string file_name);
    void clear_file();
    Logger::ptr Getlogger() { return logger_m; }

private:
    LogLevel::Level level = LogLevel::Level::DEBUG;
    LogEvent::ptr event;
    LogFormatter::ptr logformate;
    Logger::ptr logger_m;
    //要输出的文件集合
    std::vector<std::string> file = {"test.txt"};
};

}

#endif



