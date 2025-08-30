#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <DebugLog.h>

namespace fs = std::filesystem;

class DebugLogTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (fs::exists("logs")) {
            fs::remove_all("logs");
        }
    }

    void TearDown() override {
        Debug::Shutdown();

        if (fs::exists("logs")) {
            fs::remove_all("logs");
        }
    }

    static std::string ReadFile(const fs::path& file) {
        const std::ifstream in(file);
        std::stringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }
};

TEST_F(DebugLogTest, CreatesLogDirectoriesAndFiles) {
    Debug::Log("Hello log system");

    ASSERT_TRUE(fs::exists("logs/all")) << "logs/all should exist";
    ASSERT_TRUE(fs::exists("logs/errors")) << "logs/errors should exist";

    auto allLogs = fs::directory_iterator("logs/all");
    ASSERT_TRUE(allLogs != fs::end(allLogs)) << "logs/all should contain files";

    auto errLogs = fs::directory_iterator("logs/errors");
    ASSERT_TRUE(errLogs != fs::end(errLogs)) << "logs/errors should contain files";
}

TEST_F(DebugLogTest, WritesToAllLog) {
    Debug::Log("Message A");

    auto allLogs = *fs::directory_iterator("logs/all");
    std::string content = ReadFile(allLogs.path());

    EXPECT_NE(content.find("Message A"), std::string::npos);
}

TEST_F(DebugLogTest, WritesWarningToAllAndErrorLogs) {
    Debug::LogWarning("Warning message");

    auto allLogs = *fs::directory_iterator("logs/all");
    auto errLogs = *fs::directory_iterator("logs/errors");

    std::string allContent = ReadFile(allLogs.path());
    std::string errContent = ReadFile(errLogs.path());

    EXPECT_NE(allContent.find("Warning message"), std::string::npos);
    EXPECT_NE(errContent.find("Warning message"), std::string::npos);
}

TEST_F(DebugLogTest, WritesErrorToAllAndErrorLogs) {
    Debug::LogError("Error message");

    auto allLogs = *fs::directory_iterator("logs/all");
    auto errLogs = *fs::directory_iterator("logs/errors");

    std::string allContent = ReadFile(allLogs.path());
    std::string errContent = ReadFile(errLogs.path());

    EXPECT_NE(allContent.find("Error message"), std::string::npos);
    EXPECT_NE(errContent.find("Error message"), std::string::npos);
}

TEST_F(DebugLogTest, ThreadSafetyTest) {
    constexpr int kThreads = 8;
    constexpr int kMessagesPerThread = 20;

    auto worker = [&]() {
        for (int i = 0; i < kMessagesPerThread; ++i) {
            Debug::Log("Threaded message");
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(kThreads);

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back(worker);
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    const auto allLogs = *fs::directory_iterator("logs/all");
    std::string content = ReadFile(allLogs.path());

    int count = 0;
    size_t pos = content.find("Threaded message");
    while (pos != std::string::npos) {
        count++;
        pos = content.find("Threaded message", pos + 1);
    }

    EXPECT_EQ(count, kThreads * kMessagesPerThread);
}
