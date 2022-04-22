#include <iostream>
#include "log.h"

namespace zyx
{
    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
        , const char* m_file, int32_t m_line, uint32_t m_elapse, uint32_t m_threadId,
        uint32_t m_fiberId, uint64_t m_time, std::string m_threadName)
    {
        this->m_file = m_file;
        this->m_line = m_line;
        this->m_elapse = m_elapse;
        this->m_threadId = m_threadId;
        this->m_fiberId = m_fiberId;
        this->m_time = m_time;
        this->m_threadName = m_threadName;
        //this->m_FiberName=m_FiberName;
        m_logger = logger;
        m_level = level;
    }

    const char* LogLevel::ToString(LogLevel::Level level) {
        switch (level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
    }

    LogLevel::Level LogLevel::FromString(const std::string& str) {
#define XX(level, v) \
    if(str == #v) { \
        return LogLevel::level; \
    }
        XX(DEBUG, debug);
        XX(INFO, info);
        XX(WARN, warn);
        XX(ERROR, error);
        XX(FATAL, fatal);

        XX(DEBUG, DEBUG);
        XX(INFO, INFO);
        XX(WARN, WARN);
        XX(ERROR, ERROR);
        XX(FATAL, FATAL);
        return LogLevel::UNKNOW;
#undef XX
    }
    std::ostream& LogFormatter::format(std::ostream& ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        int s_size = m_items.size();
        for (auto i : m_items)
        {
            i->format(ofs, logger, level, event);
        }
        return ofs;
    }

    Logger::Logger(LogLevel::Level my_level, LogFormatter::ptr m_formatter)
    {
        this->my_level = my_level;
        //this->m_formatter = new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= my_level)
        {
            auto self = shared_from_this();
            if (!m_appenders.empty())
            {
                for (auto i : m_appenders)
                {
                    i->log(self,level,event);
                }
            }
        }
    }
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }
    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_appenders.push_back(appender);
    }

    void StdoutLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    {
        mutex.lock();
        m_formatter->format(std::cout, logger, level,event);  
        mutex.unlock();
    }
    FileLogAppender::FileLogAppender(const std::string filename, LogFormatter::ptr p)
    {
        this->m_filename = filename;
        m_formatter = p;
    }
    void FileLogAppender::log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event)
    { 
        mutex.lock();
        FileLogAppender::m_filestream.open(m_filename, std::ios::out | std::ios::app );
        m_formatter->format(m_filestream, logger, level,event);
        m_filestream.close();
        mutex.unlock();
    }
    class ElseFormatItem :public LogFormatter::FormatItem
    {
    public:
        ElseFormatItem(char c)
        {
            p = c;
        }
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << p;
        }
    private:
        char p;
    };
    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        //MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        //LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
       // ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
    };



    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        //ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        //FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        //ThreadNameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadName();
        }
    };
    // class FiberNameFormatItem : public LogFormatter::FormatItem {
    // public:
    //     void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
    //         os << event->getFiberName();
    //     }
    // };

    class DateTimeFormatItem : public LogFormatter::FormatItem 
    {
    public:
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override 
        {
            time_t now_time = time(NULL);
            tm* t_tm = localtime(&now_time);
            char* time_str = asctime(t_tm);
            //std::string ss = time_str;
            os << t_tm->tm_mon+1<<"-"<<t_tm->tm_mday<<" "<<t_tm->tm_hour<<":"<<t_tm->tm_min<<":"<<t_tm->tm_sec;
        }
    };

    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        //FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        //LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        //NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };


    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string& str)
            :m_string(str) {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string;
        }
    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        //TabFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }
    private:
        std::string m_string;
    };
    void LogFormatter::init()
    {
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }
        }
        
         static std::map<char, LogFormatter::FormatItem::ptr> mark = { {'p',FormatItem::ptr(new LevelFormatItem)},
                                                {'r',FormatItem::ptr(new ElapseFormatItem)},
                                                {'m',FormatItem::ptr(new MessageFormatItem)}, 
                                                {'t',FormatItem::ptr(new ThreadIdFormatItem)},
                                                {'n',FormatItem::ptr(new NewLineFormatItem)},
                                                {'f',FormatItem::ptr(new FilenameFormatItem)},
                                                {'l',FormatItem::ptr(new  LineFormatItem)},
                                                {'N',FormatItem::ptr(new ThreadNameFormatItem)},
                                               // {'B',FormatItem::ptr(new FiberNameFormatItem)},
                                                {'T',FormatItem::ptr(new TabFormatItem)},
                                                {'F',FormatItem::ptr(new FiberIdFormatItem)},
                                                {'D',FormatItem::ptr(new DateTimeFormatItem)} };
         for (char i : nstr)
         {
             if (mark.find(i) == mark.end())
             {
                 m_items.push_back(FormatItem::ptr(new ElseFormatItem(i)));
             }
             else
                m_items.push_back(mark[i]);
         }

    }

    LogFormatter::LogFormatter(const std::string formate)
    {
        m_pattern = formate;
        init();
    }
    
    LoggerManager::LoggerManager( LogLevel::Level level, bool write_std, bool write_file)
    {
        zyx::LogFormatter::ptr format_ptr(new zyx::LogFormatter());
        logformate= format_ptr;
        zyx::Logger::ptr logger(new zyx::Logger(level, logformate));
        logger_m = logger;
        this->level = level;
        if (write_std == true)
        {
            zyx::LogAppender::ptr aptr(new zyx::StdoutLogAppender(logformate));
            logger_m->addAppender(aptr);
        }
        if (write_file == true)
        {
            int f_size = file.size();
            for (auto f : file)
            {
                zyx::LogAppender::ptr fptr(new zyx::FileLogAppender(f, logformate));
                logger_m->addAppender(fptr);
            }
        }
    }
    LoggerManager::LoggerManager(LogLevel::Level level, bool write_std, std::vector<std::string>write_file,std::string formate )
    {
        zyx::LogFormatter::ptr format_ptr(new zyx::LogFormatter(formate));
        logformate = format_ptr;
        zyx::Logger::ptr logger(new zyx::Logger(level, logformate));
        logger_m = logger;
        this->level = level;
        if (write_std == true)
        {
            zyx::LogAppender::ptr aptr(new zyx::StdoutLogAppender(logformate));
            logger_m->addAppender(aptr);
        }
       
        clear_file();
        for (auto s : write_file)
        {
            file.push_back(s);
        }
        int f_size = file.size();
        for (auto f : file)
        {
            zyx::LogAppender::ptr fptr(new zyx::FileLogAppender(f, logformate));
            logger_m->addAppender(fptr);
        }
        
    }
   
    void LoggerManager::add_file(std::string file_name)
    {
        file.push_back(file_name);
    }
    void LoggerManager::clear_file()
    {
        file.clear();
    }

}