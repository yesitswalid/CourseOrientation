#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "ilogger.h"
#include <QFile>
#include <QMutex>
#include <memory>

class FileLogger : public ILogger {
public:
    static FileLogger &instance();

    bool initialize(const QString &filePath);

    void log(LogLevel level, const QString &message) override;
    void debug(const QString &msg)    override { log(LogLevel::Debug,    msg); }
    void info(const QString &msg)     override { log(LogLevel::Info,     msg); }
    void warning(const QString &msg)  override { log(LogLevel::Warning,  msg); }
    void error(const QString &msg)    override { log(LogLevel::Error,    msg); }
    void critical(const QString &msg) override { log(LogLevel::Critical, msg); }

private:
    FileLogger() = default;
    ~FileLogger() = default;
    FileLogger(const FileLogger &) = delete;
    FileLogger &operator=(const FileLogger &) = delete;

    QString formatMessage(LogLevel level, const QString &message) const;
    QString levelToString(LogLevel level) const;

    std::unique_ptr<QFile> m_file;
    mutable QMutex m_mutex;
};

#endif // FILELOGGER_H
