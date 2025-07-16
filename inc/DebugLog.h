#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

#include <string>
#include <mutex>
#include <fstream>
#include <sstream>
#include <utility>
#include <type_traits>
#include <fmt/core.h>

#ifndef NO_DISCARD
#define NO_DISCARD [[nodiscard]]
#endif

class Debug {
public:

#if (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
    template <typename... Args>
    static void Log(fmt::format_string<Args...> fmt, Args&&... args) {
        Log(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::DEFAULT_DEBUG_LOG);
    }

    template <typename... Args>
    static void LogWarning(fmt::format_string<Args...> fmt, Args&&... args) {
        Log(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::WARNING_DEBUG_LOG);
    }

    template <typename... Args>
    static void LogError(fmt::format_string<Args...> fmt, Args&&... args) {
        Log(fmt::vformat(fmt, fmt::make_format_args(args...)), DebugLogType_::ERROR_DEBUG_LOG);
    }
#else
    // Fallback for older C++ standards
    template <typename S, typename... Args>
    static void Log(const S& format_str, Args&&... args) {
        Log(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::DEFAULT_DEBUG_LOG);
    }

    template <typename S, typename... Args>
    static void LogWarning(const S& format_str, Args&&... args) {
        Log(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::WARNING_DEBUG_LOG);
    }

    template <typename S, typename... Args>
    static void LogError(const S& format_str, Args&&... args) {
        Log(fmt::format(format_str, std::forward<Args>(args)...), DebugLogType_::ERROR_DEBUG_LOG);
    }
#endif


private:
    enum class DebugLogType_ {
        DEFAULT_DEBUG_LOG,
        WARNING_DEBUG_LOG,
        ERROR_DEBUG_LOG
    };

    static NO_DISCARD Debug& CreateInstance();
    static const char* LogTypeToString(DebugLogType_ type);
    static void Log(const std::string& message, DebugLogType_ type);
    static void Init();
    static std::string GetTimestamp();

    static Debug* m_instance;

    std::mutex m_mutex;
    std::ofstream m_fileLogStream;
    std::ofstream m_fileLogErrorStream;
    static std::once_flag m_initFlag;
};

#endif // DEBUG_LOG_H