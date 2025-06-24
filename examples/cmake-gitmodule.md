# 🔗 Git Submodule Integration

### Step 1: Add the Debug-Log Submodule
```shell
$ mkdir ext
$ cd ext
$ git submodule add https://github.com/ddj4747/Debug-Log
$ cd ..
```

### Step 2: Update Your Root CMakeLists.txt With
```cmake
# Add the Debug-Log submodule
add_subdirectory(ext/Debug-Log)

# Link Debug-Log to your target
target_link_libraries(YourTarget PRIVATE Debug-Log)
```

### 🛠 Don’t Forget

After cloning your repo elsewhere, initialize submodules:
``` shell
$ git submodule update --init --recursive
```