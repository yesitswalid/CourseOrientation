#include "filelogger.h"
#include <QDateTime>
#include <QMutexLocker>
#include <QTextStream>

FileLogger &FileLogger::instance()
{
    static FileLogger inst;
    return inst;
}

bool FileLogger::initialize(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    m_file = std::make_unique<QFile>(filePath);
    return m_file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}

void FileLogger::log(LogLevel level, const QString &message)
{
    QMutexLocker locker(&m_mutex);
    if (!m_file || !m_file->isOpen())
        return;
    QTextStream out(m_file.get());
    out << formatMessage(level, message) << "\n";
    out.flush();
}

QString FileLogger::formatMessage(LogLevel level, const QString &message) const
{
    return QString("[%1] [%2] %3")
        .arg(QDateTime::currentDateTime().toString(Qt::ISODate))
        .arg(levelToString(level))
        .arg(message);
}

QString FileLogger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug:    return "DEBUG";
    case LogLevel::Info:     return "INFO";
    case LogLevel::Warning:  return "WARN";
    case LogLevel::Error:    return "ERROR";
    case LogLevel::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}
