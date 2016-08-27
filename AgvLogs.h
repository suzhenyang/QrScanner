#ifndef AGVLOGS_H
#define AGVLOGS_H

#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/PatternLayout.hh>

using namespace std;

class CAgvLogs
{
public:
    CAgvLogs();
    ~CAgvLogs();

public:
    void Initial();
    void SaveInfo(string info);
    void SaveError(string error);
    void SaveDebug(string debug);

    static string ByteArrayToString(unsigned char *data, int len);

private:
    log4cpp::Category* logger;
};


#endif // AGVLOGS_H
