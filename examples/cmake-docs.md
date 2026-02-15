# Debug-Log

Debug-Log is a lightweight C++ logging utility with printf-style formatting, automatic file logging, and optional stack traces using the Boost library.

---

## ✅ Getting Started

To use Debug-Log, simply include the header:

```cpp
#include <DebugLog.h>
```

---

## 📁 Directory Structure

Logs are saved to the following locations:

```
logs/
├── all/     # All log messages
└── errors/  # Warning and error messages
```

Each run creates a new log file named with the current timestamp (e.g., `2025-06-24_12-34-56.log`).

## 📝 Logging Functions

Debug-Log uses the `{fmt}` library for printf-style formatting.

### `Debug::Log()`

Logs a standard informational message.

**Saved to:** `logs/all/<timestamp>.log`

```cpp
#include <DebugLog.h>
#include <string>

int main() {
    const char* message1 = "Message1";
    std::string message2 = "Message2";
    int value = 42;

    Debug::Log("This is a standard log message.");
    Debug::Log("This is message1: {}", message1);
    Debug::Log("This is message2: {}, and a number: {}", message2, value);

    return 0;
}
```

**Output:**
```
[LOG 2025-06-24_12-34-56] This is a standard log message.
[LOG 2025-06-24_12-34-56] This is message1: Message1
[LOG 2025-06-24_12-34-56] This is message2: Message2, and a number: 42
```

---

### ⚠️ `Debug::LogWarning()`

Logs a warning about a non-critical issue and includes a stack trace.

**Saved to:**
- `logs/all/<timestamp>.log`
- `logs/errors/<timestamp>.log`

```cpp
#include <DebugLog.h>
#include <string>

void someFunction() {
    Debug::LogWarning("Something unexpected happened here.");
}

int main() {
    someFunction();
    return 0;
}
```

**Output:**
```
[WARNING 2025-06-24_12-34-56] Something unexpected happened here.
Stacktrace (
  0# someFunction() at /path/to/your/project/main.cpp:8
  1# main at /path/to/your/project/main.cpp:12
)
```

---

### ❌ `Debug::LogError()`

Logs a critical error message and includes a stack trace.

**Saved to:**
- `logs/all/<timestamp>.log`
- `logs/errors/<timestamp>.log`

```cpp
#include <DebugLog.h>
#include <string>

void criticalOperation() {
    Debug::LogError("A critical error occurred!");
}

int main() {
    criticalOperation();
    return 0;
}
```

**Output:**
```
[ERROR 2025-06-24_12-34-56] A critical error occurred!
Stacktrace (
  0# criticalOperation() at /path/to/your/project/main.cpp:8
  1# main at /path/to/your/project/main.cpp:12
)
```

---

## 🔧 Logger Configuration

### Logger behavior can be customized via:
``` cpp
Debug::SetSettings(const Debug::Settings& settings);
```
### Available Options

| Option            | Description                                                                                                 |
|-------------------|-------------------------------------------------------------------------------------------------------------|
| rootPath          | Directory where log files are stored. If it does not exist, it will be created automatically.               |
| maxFileSize       | Maximum size (in bytes) of a single .log file. When exceeded, a new log file is created.                    |
| maxLogFilesAmount | Maximum number of log files retained in rootPath. When the limit is exceeded, the oldest files are removed. |
| deleteLogsAfter   | Maximum lifetime (in seconds) of a log file. Files older than this value are automatically deleted.         |

## ⚙️ CMake Configuration

DebugLog supports several CMake options to customize logging behavior:

| Option | Description |
|--------|-------------|
| `DEBUG_LOG_DISABLE_LOGGING` | Disables all logging functionality entirely. |
| `DEBUG_LOG_DISABLE_CONSOLE_LOGGING` | Prevents logs from being printed to the console. |
| `DEBUG_LOG_DISABLE_FILE_LOGGING` | Prevents logs from being written to log files. |
| `DEBUG_LOG_DISABLE_STACKTRACE` | Disables stack trace generation for warnings and errors. |

To set an option, add to your `CMakeLists.txt`:

```cmake
set(DEBUG_LOG_DISABLE_CONSOLE_LOGGING ON)
```

---

## 📌 Notes

- Stack traces for `Debug::LogWarning()` and `Debug::LogError()` require the Boost library. If Boost is not found, stack trace generation is disabled.
- Timestamps follow the format: `YYYY-MM-DD_HH-MM-SS`.
- Logging functions accept both `const char*` and `std::string`, and support `{fmt}`-style format strings.
- Ensure the `logs/` directory is writable by the application.
- Log file creation is deferred until the first log call.
- If opening a log file fails, the program will throw a `std::runtime_error`.
