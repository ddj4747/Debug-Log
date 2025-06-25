# Debug Log
A lightweight, thread-safe logging utility for C++ that supports colored console output, timestamped log entries, and structured file logging. Ideal for both development and production environments.

## 🔧 Build Instructions

```shell
$ git clone https://github.com/ddj4747/Debug-Log.git
$ cd Debug-Log
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

## ✨ Features

- Thread-safe logging using `std::mutex` and `std::lock_guard`
- Supports `const char*` and `std::string` input
- Console output with color (using `fmt`)
- File output:
    - All logs stored in `logs/all/`
    - Warnings and errors also stored in `logs/errors/`
- Log entries are timestamped

## 📌 Dependencies

- [fmt](https://github.com/fmtlib/fmt) for formatted and colored console output

## 📁 Directory Structure

Logs are saved to the following locations:
``` shell
logs/
├── all/ # All log messages
└── errors/ # Warning and error messages
```

Each run creates log files with the current timestamp as the filename.

## 🚀 Usage

### Basic Logging

```c++
Debug::Log("This is a standard log message.");
Debug::LogWarning("This is a warning.");
Debug::LogError("This is an error message.");

// You can also pass std::string:

std::string message = "Log message";
Debug::Log(message);
```

### Example console output
```shell
$ [LOG     2025-06-24_12-34-56] This is a standard log message.
$ [WARNING 2025-06-24_12-34-56] This is a warning.
$ [ERROR   2025-06-24_12-34-56] This is an error message.
```
### Log files

- All messages are saved in:```logs/all/<timestamp>.log```

- Warnings and errors are also saved in: ```logs/errors/<timestamp>.log```

## 📚 Additional
- [FetchContent example](examples/cmake-fetchcontent.md)
- [Git Submodule example](examples/cmake-gitmodule.md)

## ⚠️ Notes
- Ensure logs/ is writable by the application.

- Log file creation is deferred until the first log call.

- If file opening fails, the program will throw an exception.
