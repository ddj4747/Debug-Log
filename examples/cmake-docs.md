# 📄 Debug-Log Documentation

## ✅ Getting Started
To use Debug-Log, simply include the header:
```c++
#include <DebugLog.h>
```

## 📁 Directory Structure

Logs are saved to the following locations:
``` shell
logs/
├── all/ # All log messages
└── errors/ # Warning and error messages
```
Each run creates a new log file named with the current timestamp (e.g., ```2025-06-24_12-34-56.log```).


## 📝 Logging Functions

### Debug::Log()
Logs a standard informational message.
Messages are saved to: ```logs/all/<timestamp>.log```.

```c++
#include <DebugLog.h>
#include <string>

int main() {
    const char* message1 = "Message1";
    std::string message2 = "Message2";
    
    Debug::Log(message1);
    Debug::Log(message2);
    
    return 0;
}
```

Output: 
```shell
$ [LOG     2025-06-24_12-34-56] Message1
$ [LOG     2025-06-24_12-34-56] Message2
```

### ⚠️ Debug::LogWarning()
Logs a warning about a non-critical issue. Messages are saved to:
- ```logs/all/<timestamp>.log```
- ```logs/errors/<timestamp>.log```


```c++
#include <DebugLog.h>
#include <string>

int main() {
    const char* message1 = "Message1";
    std::string message2 = "Message2";
    
    Debug::LogWarning(message1);
    Debug::LogWarning(message2);
    
    return 0;
}
```

Output:
```shell
$ [WARNING 2025-06-24_12-34-56] Message1
$ [WARNING 2025-06-24_12-34-56] Message2
```

### ❌ Debug::LogError()
Logs a critical error message. Messages are saved to:
- ```logs/all/<timestamp>.log```
- ```logs/errors/<timestamp>.log```

```c++
#include <DebugLog.h>
#include <string>

int main() {
    const char* message1 = "Message1";
    std::string message2 = "Message2";
    
    Debug::LogError(message1);
    Debug::LogError(message2);
    
    return 0;
}
```

Output:
```shell
$ [ERROR   2025-06-24_12-34-56] Message1
$ [ERROR   2025-06-24_12-34-56] Message2
```

## ⚙️ CMake Configuration
DebugLog supports several CMake options for customizing logging behavior:

### 🔇 Disable All Logging
Disables all logging functionality (no output at all):
```cmake
set(DEBUG_LOG_DISABLE_LOGGING ON)
```

### 🖥️ Disable Console Logging
Prevents logs from being printed to the console:
```cmake
set(DEBUG_LOG_DISABLE_CONSOLE_LOGGING ON)
```

### 📁 Disable File Logging
Prevents logs from being written to a log file:
```cmake
set(DEBUG_LOG_DISABLE_FILE_LOGGING ON)
```


## 📌 Notes
- Timestamps follow the format: ```YYYY-MM-DD_HH-MM-SS```.
- logging functions accept both ```const char*``` and ```std::string```.
- Ensure logs/ is writable by the application.
- Log file creation is deferred until the first log call.
- If file opening fails, the program will throw an exception.
- All messages are saved in:```logs/all/<timestamp>.log```
- Warnings and errors are also saved in: ```logs/errors/<timestamp>.log```
