#include <DebugLog.h>
#include <filesystem>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <queue>

#include "fmt/os.h"

#ifndef DISABLE_LOGGING_STACKTRACE
#include <boost/stacktrace.hpp>
#endif

#include <fmt/color.h>
#include <fmt/ostream.h>
#include <utf8.h>

inline std::string sanitizeUtf8(const std::string& str) {
    if (utf8::is_valid(str)) return str;
    return utf8::replace_invalid(str, U'\uFFFD');
}

std::mutex Debug::m_mutex{};
std::ofstream Debug::m_fileLogStream{};
std::ofstream Debug::m_fileLogErrorStream{};
bool Debug::m_initFlag{};
Debug::Settings Debug::m_settings{
    "",
    2 * 1024 * 1024,
    10,
    60 * 60 * 24 * 7
};
size_t Debug::m_currentLogStreamFileSize{};
size_t Debug::m_currentLogErrorStreamFileSize{};

const char* Debug::LogTypeToString(const DebugLogType_ type) {
    switch (type) {
        case DebugLogType_::DEFAULT_DEBUG_LOG: return "LOG";
        case DebugLogType_::WARNING_DEBUG_LOG: return "WARNING";
        case DebugLogType_::ERROR_DEBUG_LOG:   return "ERROR";
    }
    return "UNKNOWN";
}

void Debug::LogI(const std::string& message, const DebugLogType_ type) {
#ifndef DISABLE_LOGGING
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_initFlag) {
            m_initFlag = true;
            Init();
        }

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
            fmt::print("{}\n", sanitizeUtf8(formatted));
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
            m_currentLogStreamFileSize += formatted.size() + 1;
            m_fileLogStream << formatted << std::endl;
#endif // !DISABLE_FILE_LOGGING
            break;

        case DebugLogType_::WARNING_DEBUG_LOG:
#ifndef DISABLE_CONSOLE_LOGGING
            fmt::print(fg(fmt::color::yellow), "{}\n", sanitizeUtf8(formatted));
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
            m_currentLogStreamFileSize += formatted.size() + 1;
            m_currentLogErrorStreamFileSize += formatted.size() + 1;
            m_fileLogStream << formatted << std::endl;
            m_fileLogErrorStream << formatted << std::endl;
#endif // !DISABLE_FILE_LOGGING
            break;

        case DebugLogType_::ERROR_DEBUG_LOG:
#ifndef DISABLE_CONSOLE_LOGGING
            fmt::print(fg(fmt::color::red), "{}\n", sanitizeUtf8(formatted));
#endif // !DISABLE_CONSOLE_LOGGING
#ifndef DISABLE_FILE_LOGGING
            m_currentLogStreamFileSize += formatted.size() + 1;
            m_currentLogErrorStreamFileSize += formatted.size() + 1;
            m_fileLogStream << formatted << std::endl;
            m_fileLogErrorStream << formatted << std::endl;
#endif // !DISABLE_FILE_LOGGING
            break;
        }

#ifndef DISABLE_FILE_LOGGING
        if (m_currentLogStreamFileSize < m_settings.maxFileSize && m_currentLogErrorStreamFileSize < m_settings.maxFileSize) {
            return;
        }
#endif // !DISABLE_FILE_LOGGING
    }

#ifndef DISABLE_FILE_LOGGING
    Shutdown();
#endif // !DISABLE_FILE_LOGGING

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

std::chrono::time_point<std::chrono::system_clock> Debug::ParseTimestamp(const std::string_view str) {
    std::tm tm{};
    std::istringstream iss((str.data()));
    iss >> std::get_time(&tm, "%Y-%m-%d_%H-%M-%S");

    if (iss.fail())
        throw std::runtime_error("Invalid timestamp format");

    const std::time_t tt = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(tt);
}

void Debug::SetSettings(const Settings& settings) {
    std::lock_guard<std::mutex> lock(m_mutex);

    m_settings = settings;
    if (m_fileLogStream.is_open() || m_fileLogErrorStream.is_open()) {
        Shutdown();
    }

    m_initFlag = true;
    Init();
}

void Debug::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_fileLogStream.is_open()) m_fileLogStream.close();
    if (m_fileLogErrorStream.is_open()) m_fileLogErrorStream.close();
    m_initFlag = false;
}


void Debug::Init() {
    if (!std::filesystem::is_directory(std::filesystem::path("logs/"))) {
        std::filesystem::create_directory(std::filesystem::path("logs/"));
    }

    const std::filesystem::path allLogsRoot = std::filesystem::path(m_settings.rootPath / "logs/all/");
    const std::filesystem::path errorLogsRoot = std::filesystem::path(m_settings.rootPath / "logs/errors/");

    std::filesystem::create_directories(allLogsRoot);
    std::filesystem::create_directories(errorLogsRoot);

    const std::string fileName = GetTimestamp() + ".log";
    const std::filesystem::path allLogPath(allLogsRoot / fileName);
    const std::filesystem::path errorLogPath(errorLogsRoot / fileName);

    m_fileLogStream.open(allLogPath, std::ios::out | std::ios::app);
    m_fileLogErrorStream.open(errorLogPath, std::ios::out | std::ios::app);

    ClearLogs(allLogsRoot);
    ClearLogs(errorLogsRoot);

    if (!m_fileLogStream.is_open() || !m_fileLogErrorStream.is_open()) {
        throw std::runtime_error("Failed to open log files.");
    }
}

void Debug::ClearLogs(const std::filesystem::path& rootPath) {
    std::priority_queue<
        std::pair<std::chrono::time_point<std::chrono::system_clock>, std::string>,
        std::vector<std::pair<std::chrono::system_clock::time_point, std::string>>,
        std::greater<>
    > logFilesNames;
    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

    for (const auto& file : std::filesystem::directory_iterator(rootPath)) {
        if (!file.is_regular_file())
            continue;

        if (file.path().extension() != ".log")
            continue;

        try {
            std::string fileName = file.path().filename().string();
            std::chrono::time_point<std::chrono::system_clock> timestamp = ParseTimestamp(fileName.substr(0, fileName.size()-4));

            auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
            if (diff.count() > m_settings.deleteLogsAfter) {
                std::filesystem::remove(file);
                continue;
            }

            logFilesNames.emplace(timestamp, fileName);
        }  catch (...) { }
    }

    while (logFilesNames.size() > m_settings.maxLogFilesAmount) {
        std::filesystem::remove(rootPath / logFilesNames.top().second);
        logFilesNames.pop();
    }
}
