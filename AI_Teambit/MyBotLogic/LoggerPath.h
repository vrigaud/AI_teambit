#ifndef LOGGER_PATH_H
#define LOGGER_PATH_H

#include "Singleton.h"
#include <string>


class LoggerPath : Singleton
{
    std::string mPath;


public:
    static LoggerPath* getInstance()
    {
        static LoggerPath instance;
        return &instance;
    }

    void setPath(std::string str)
    {
        mPath = str;
    }

    std::string getPath()
    {
        return mPath;
    }
};
#endif // LOGGER_PATH_H
