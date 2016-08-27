#include "AgvLogs.h"

CAgvLogs::CAgvLogs()
{
    //1. 初始化一个layout 对象
    log4cpp::PatternLayout* pLayout = new log4cpp::PatternLayout();
    pLayout->setConversionPattern("%d: %p %c %x: %m%n");
    // 2. 初始化一个appender 对象
    log4cpp::Appender* appender = new log4cpp::RollingFileAppender( "rollfileAppender","rollwxb.log",1*1024*1024, 1);
    // 3. 把layout对象附着在appender对象上
    appender->setLayout(pLayout);
    // 4. 实例化一个category对象
    logger = &log4cpp::Category::getRoot();
    // 5. 设置additivity为false，替换已有的appender
    logger->setAdditivity(false);
    // 5. 把appender对象附到category上
    logger->setAppender(appender);
    // 6. 设置category的优先级，低于此优先级的日志不被记录
    logger->setPriority(log4cpp::Priority::INFO);
}

CAgvLogs::~CAgvLogs()
{
    log4cpp::Category::shutdown();
}


void CAgvLogs::Initial()
{

}

void CAgvLogs::SaveInfo(string info)
{
    logger->info(info);
}

void CAgvLogs::SaveDebug(string debug)
{
    logger->debug(debug);
}

void CAgvLogs::SaveError(string error)
{
    logger->error(error);
}

string CAgvLogs::ByteArrayToString(unsigned char *data, int len)
{
    const char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7','8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    string str(len * 3-1, '-');

    for (int i = 0; i < len; ++i)
    {
        str[3 * i]     = hexmap[(data[i] & 0xF0) >> 4] ;
        str[3 * i + 1] = hexmap[data[i] & 0x0F];

        if ( (3*i+2) != (3*len-1) )
        {
            str[3 * i + 2] = '-';
        }

    }

    return str;
}





