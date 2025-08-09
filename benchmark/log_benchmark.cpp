#include <benchmark/benchmark.h>
#include <DebugLog.h>
#include <thread>

static void BM_Log_StringView(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Test message");
    }
}
BENCHMARK(BM_Log_StringView);

static void BM_Log_String(benchmark::State& state) {
    std::string msg = "Test message";
    for (auto _ : state) {
        Debug::Log(msg);
    }
}
BENCHMARK(BM_Log_String);

static void BM_Log_Integer(benchmark::State& state) {
    int value = 42;
    for (auto _ : state) {
        Debug::Log(value);
    }
}
BENCHMARK(BM_Log_Integer);

static void BM_Log_Formatted(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Value: {}", 42);
    }
}
BENCHMARK(BM_Log_Formatted);

static void BM_LogWarning_String(benchmark::State& state) {
    for (auto _ : state) {
        Debug::LogWarning("Warning message");
    }
}
BENCHMARK(BM_LogWarning_String);

static void BM_LogError_String(benchmark::State& state) {
    for (auto _ : state) {
        Debug::LogError("Error message");
    }
}
BENCHMARK(BM_LogError_String);


// ===============================
// Multi-threaded benchmarks
// ===============================

static void BM_Log_MultiThread(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Multi-thread log test");
    }
}
BENCHMARK(BM_Log_MultiThread)->ThreadRange(1, std::thread::hardware_concurrency());

static void BM_Log_Formatted_MultiThread(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Thread {} logging value {}", std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())), 42);
    }
}
BENCHMARK(BM_Log_Formatted_MultiThread)->ThreadRange(1, std::thread::hardware_concurrency());

BENCHMARK_MAIN();
