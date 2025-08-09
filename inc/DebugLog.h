#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <string>
#include <mutex>
#include <fstream>
#include <sstream>
#include <utility>
#include <type_traits>
#include <fmt/core.h>
#include <string_view>

#ifndef NO_DISCARD
#define NO_DISCARD [[nodiscard]]
#endif

class Debug {
public:

    static void Log(const std::string_view value) {
        LogI(std::string(value), DebugLogType_::DEFAULT_DEBUG_LOG);
    }
    static void LogWarning(const std::string_view value) {
        LogI(std::string(value), DebugLogType_::WARNING_DEBUG_LOG);
    }
    static void LogError(const std::string_view value) {
        LogI(std::string(value), DebugLogType_::ERROR_DEBUG_LOG);
    }

    template <typename T, typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
    static void Log(const T& value) {
        LogI(fmt::format("{}", value), DebugLogType_::DEFAULT_DEBUG_LOG);
    }

    template <typename T, typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
    static void LogWarning(const T& value) {
        LogI(fmt::format("{}", value), DebugLogType_::WARNING_DEBUG_LOG);
    }

    template <typename T, typename = std::enable_if_t<!std::is_convertible_v<T, std::string_view>>>
    static void LogError(const T& value) {
        LogI(fmt::format("{}", value), DebugLogType_::ERROR_DEBUG_LOG);
    }

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    template <typename... Args>
    static void Log(fmt::format_string<Args...> fmt, Args&&... args) {
        LogI(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::DEFAULT_DEBUG_LOG);
    }

    template <typename... Args>
    static void LogWarning(fmt::format_string<Args...> fmt, Args&&... args) {
        LogI(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::WARNING_DEBUG_LOG);
    }

    template <typename... Args>
    static void LogError(fmt::format_string<Args...> fmt, Args&&... args) {
        LogI(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::ERROR_DEBUG_LOG);
    }
#else
    // Fallback for older C++ standards
    template <typename S, typename... Args>
    static void Log(const S& format_str, Args&&... args) {
        LogI(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::DEFAULT_DEBUG_LOG);
    }

    template <typename S, typename... Args>
    static void LogWarning(const S& format_str, Args&&... args) {
        LogI(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::WARNING_DEBUG_LOG);
    }

    template <typename S, typename... Args>
    static void LogError(const S& format_str, Args&&... args) {
        LogI(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::ERROR_DEBUG_LOG);
    }
#endif

private:
    enum class DebugLogType_ {
        DEFAULT_DEBUG_LOG,
        WARNING_DEBUG_LOG,
        ERROR_DEBUG_LOG
    };

    static const char* LogTypeToString(DebugLogType_ type);
    static void LogI(const std::string& message, DebugLogType_ type);
    static void Init();
    static std::string GetTimestamp();

    static std::mutex m_mutex;
    static std::ofstream m_fileLogStream;
    static std::ofstream m_fileLogErrorStream;
    static std::once_flag m_initFlag;
};

#endif // DEBUG_LOG_H
