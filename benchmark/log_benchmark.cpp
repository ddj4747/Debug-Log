#include <benchmark/benchmark.h>
#include <DebugLog.h>

static void BM_Log_StringView(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Test message");
    }
}

static void BM_Log_String(benchmark::State& state) {
    std::string msg = "Test message";
    for (auto _ : state) {
        Debug::Log(msg);
    }
}

static void BM_Log_Integer(benchmark::State& state) {
    int value = 42;
    for (auto _ : state) {
        Debug::Log(value);
    }
}

static void BM_Log_Formatted(benchmark::State& state) {
    for (auto _ : state) {
        Debug::Log("Value: {}", 42);
    }
}

static void BM_LogWarning_String(benchmark::State& state) {
    for (auto _ : state) {
        Debug::LogWarning("Warning message");
    }
}

static void BM_LogError_String(benchmark::State& state) {
    for (auto _ : state) {
        Debug::LogError("Error message");
    }
}

BENCHMARK(BM_Log_StringView);
BENCHMARK(BM_Log_String);
BENCHMARK(BM_Log_Integer);
BENCHMARK(BM_Log_Formatted);
BENCHMARK(BM_LogWarning_String);
BENCHMARK(BM_LogError_String);

BENCHMARK_MAIN();
