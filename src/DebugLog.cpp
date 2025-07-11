#include <DebugLog.h>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

#ifndef DISABLE_LOGGING_STACKTRACE
#include <boost/stacktrace.hpp>
#endif
#include <fmt/color.h>

std::once_flag Debug::m_initFlag;
std::mutex Debug::m_mutex;
std::ofstream Debug::m_fileLogStream;
std::ofstream Debug::m_fileLogErrorStream;

void Debug::Log(const char *message) {
    Log(message, DebugLogType_::DEFAULT_DEBUG_LOG);
}

void Debug::Log(const std::string &message) {
    Log(message.c_str(), DebugLogType_::DEFAULT_DEBUG_LOG);
}

void Debug::LogWarning(const char *message) {
    Log(message, DebugLogType_::WARNING_DEBUG_LOG);
}

void Debug::LogWarning(const std::string &message) {
    Log(message.c_str(), DebugLogType_::WARNING_DEBUG_LOG);
}

void Debug::LogError(const char *message) {
    Log(message, DebugLogType_::ERROR_DEBUG_LOG);
}

void Debug::LogError(const std::string &message) {
    Log(message.c_str(), DebugLogType_::ERROR_DEBUG_LOG);
}

const char* Debug::LogTypeToString(const DebugLogType_ type) {
    switch (type) {
        case DebugLogType_::DEFAULT_DEBUG_LOG: return "LOG";
        case DebugLogType_::WARNING_DEBUG_LOG: return "WARNING";
        case DebugLogType_::ERROR_DEBUG_LOG:   return "ERROR";
    }
    return "UNKNOWN";
}

void Debug::Log(const char *message, const DebugLogType_ type) {
#ifndef DISABLE_LOGGING
    std::call_once(m_initFlag, []() {
        Init();
    });

    std::lock_guard<std::mutex> lock(m_mutex);
    const std::string timeStamp = GetTimestamp();

#ifndef DISABLE_LOGGING_STACKTRACE
    std::string formatted;
    if (type != DebugLogType_::DEFAULT_DEBUG_LOG) {
        const boost::stacktrace::stacktrace stacktrace(4, -1);
        formatted = fmt::format("[{:<8}{}] {}\nStacktrace ( \n{})", LogTypeToString(type), timeStamp, message,  boost::stacktrace::to_string(stacktrace));
    } else {
        formatted = fmt::format("[{:<8}{}] {}", LogTypeToString(type), timeStamp, message);
    }
#else
    const std::string formatted = fmt::format("[{:<8}{}] {}", LogTypeToString(type), timeStamp, message);
#endif

    switch (type) {
    case DebugLogType_::DEFAULT_DEBUG_LOG:
#ifndef DISABLE_CONSOLE_LOGGING
        fmt::print("{}\n", formatted);
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
        m_fileLogStream << formatted << '\n';
#endif // !DISABLE_FILE_LOGGING
        break;

    case DebugLogType_::WARNING_DEBUG_LOG:
#ifndef DISABLE_CONSOLE_LOGGING
        fmt::print(fg(fmt::color::yellow), "{}\n", formatted);
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
        m_fileLogStream << formatted << '\n';
        m_fileLogErrorStream << formatted << '\n';
#endif // !DISABLE_FILE_LOGGING
        break;

    case DebugLogType_::ERROR_DEBUG_LOG:
#ifndef DISABLE_CONSOLE_LOGGING
        fmt::print(fg(fmt::color::red), "{}\n", formatted);
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
        m_fileLogStream << formatted << '\n';
        m_fileLogErrorStream << formatted << '\n';
#endif // !DISABLE_FILE_LOGGING
        break;
    }
#endif // !DISABLE_LOGGING
}

std::string Debug::GetTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
    
#if defined(_WIN32)
        localtime_s(&localTime, &nowTime);
#else
        localtime_r(&nowTime, &localTime);
#endif

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void Debug::Init() {
    if (!std::filesystem::is_directory(std::filesystem::path("logs/"))) {
        std::filesystem::create_directory(std::filesystem::path("logs/"));
    }

    if (!std::filesystem::is_directory(std::filesystem::path("logs/all/"))) {
        std::filesystem::create_directory(std::filesystem::path("logs/all/"));
    }

    if (!std::filesystem::is_directory(std::filesystem::path("logs/errors"))) {
        std::filesystem::create_directory(std::filesystem::path("logs/errors"));
    }

    const std::string fileName = GetTimestamp() + ".log";
    const std::filesystem::path allLogPath("logs/all/" + fileName);
    const std::filesystem::path errorLogPath("logs/errors/" + fileName);

    m_fileLogStream.open(allLogPath, std::ios::out | std::ios::app);
    m_fileLogErrorStream.open(errorLogPath, std::ios::out | std::ios::app);

    if (!m_fileLogStream.is_open() || !m_fileLogErrorStream.is_open()) {
        throw std::runtime_error("Failed to open log files.");
    }
}
