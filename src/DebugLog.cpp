#include <DebugLog.h>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

#ifndef DISABLE_CONSOLE_LOGGING
#include <fmt/color.h>
#endif

std::once_flag Debug::m_initFlag;
std::mutex Debug::m_mutex;
std::ofstream Debug::m_fileLogStream;
std::ofstream Debug::m_fileLogErrorStream;

void Debug::Log(const char *message) {
    Log(message, LogType::DEFAULT);
}

void Debug::Log(const std::string &message) {
    Log(message.c_str(), LogType::DEFAULT);
}

void Debug::LogWarning(const char *message) {
    Log(message, LogType::WARNING);
}

void Debug::LogWarning(const std::string &message) {
    Log(message.c_str(), LogType::WARNING);
}

void Debug::LogError(const char *message) {
    Log(message, LogType::ERROR);
}

void Debug::LogError(const std::string &message) {
    Log(message.c_str(), LogType::ERROR);
}

const char* Debug::LogTypeToString(const LogType type) {
    switch (type) {
        case LogType::DEFAULT: return "LOG";
        case LogType::WARNING: return "WARNING";
        case LogType::ERROR:   return "ERROR";
    }
    return "UNKNOWN";
}

void Debug::Log(const char *message, const LogType type) {
#ifndef DISABLE_LOGGING
    std::call_once(m_initFlag, []() {
        Init();
    });

    std::lock_guard<std::mutex> lock(m_mutex);
    const std::string timeStamp = GetTimestamp();
    const std::string formatted = fmt::format("[{:<8}{}] {}", LogTypeToString(type), timeStamp, message);

    switch (type) {
        case LogType::DEFAULT:
#ifndef DISABLE_CONSOLE_LOGGING
            fmt::print("{}\n", formatted);
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
            m_fileLogStream << formatted << '\n';
#endif // !DISABLE_FILE_LOGGING
            break;

        case LogType::WARNING:
#ifndef DISABLE_CONSOLE_LOGGING
            fmt::print(fg(fmt::color::yellow), "{}\n", formatted);
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
            m_fileLogStream << formatted << '\n';
            m_fileLogErrorStream << formatted << '\n';
#endif // !DISABLE_FILE_LOGGING
            break;

        case LogType::ERROR:
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
    localtime_s(&localTime, &nowTime);

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