#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <string>
#include <mutex>
#include <fstream>

class Debug {
public:
    static void Log(const char* message);
    static void Log(const std::string& message);

    static void LogWarning(const char* message);
    static void LogWarning(const std::string& message);

    static void LogError(const char* message);
    static void LogError(const std::string& message);

private:
    enum class LogType {
        DEFAULT,
        WARNING,
        ERROR
    };

    static const char* LogTypeToString(LogType type);
    static void Log(const char* message, LogType type);
    static void Init();
    static std::string GetTimestamp();

    static std::mutex m_mutex;
    static std::ofstream m_fileLogStream;
    static std::ofstream m_fileLogErrorStream;
    static std::once_flag m_initFlag;
};

#endif // !DEBUG_LOG_H