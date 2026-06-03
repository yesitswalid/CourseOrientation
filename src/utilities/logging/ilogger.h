#ifndef ILOGGER_H
#define ILOGGER_H

#include <QString>

enum class LogLevel { Debug, Info, Warning, Error, Critical };

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const QString &message) = 0;
    virtual void debug(const QString &msg)    = 0;
    virtual void info(const QString &msg)     = 0;
    virtual void warning(const QString &msg)  = 0;
    virtual void error(const QString &msg)    = 0;
    virtual void critical(const QString &msg) = 0;
};

#endif // ILOGGER_H
