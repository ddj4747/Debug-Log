#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>
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

class DebugLogSettingsTest : public ::testing::Test {
protected:
    void SetUp() override {
        Cleanup();
    }

    void TearDown() override {
        Debug::Shutdown();
        Cleanup();
    }

    void Cleanup() {
        if (fs::exists("logs")) fs::remove_all("logs");
        if (fs::exists("custom_root")) fs::remove_all("custom_root");
    }

    void CreateDummyLog(const fs::path& directory, int secondsOffset, const std::string& content = "") {
        auto time = std::chrono::system_clock::now() + std::chrono::seconds(secondsOffset);
        std::time_t tt = std::chrono::system_clock::to_time_t(time);
        std::tm tm{};

        #if defined(_WIN32)
            localtime_s(&tm, &tt);
        #else
            localtime_r(&tt, &tm);
        #endif

        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".log";

        fs::create_directories(directory);
        std::ofstream ofs(directory / ss.str());
        ofs << (content.empty() ? "Dummy content" : content);
        ofs.close();
    }

    std::string ReadFile(const fs::path& file) {
        std::ifstream in(file);
        std::stringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }
};

TEST_F(DebugLogSettingsTest, RespectsCustomRootPath) {
    Debug::Settings settings;
    settings.rootPath = "custom_root";
    settings.maxFileSize = 1024 * 1024;
    settings.maxLogFilesAmount = 5;
    settings.deleteLogsAfter = 3600;

    Debug::SetSettings(settings);
    Debug::Log("Message in custom root");

    ASSERT_TRUE(fs::exists("custom_root/logs/all"));
    ASSERT_TRUE(fs::exists("custom_root/logs/errors"));

    auto it = fs::directory_iterator("custom_root/logs/all");
    ASSERT_TRUE(it != fs::end(it));
    EXPECT_NE(ReadFile(it->path()).find("Message in custom root"), std::string::npos);
}

TEST_F(DebugLogSettingsTest, ThresholdMessageStaysInOldFile) {
    Debug::Settings settings;
    settings.rootPath = "";
    settings.maxFileSize = 50;
    settings.maxLogFilesAmount = 5;
    settings.deleteLogsAfter = 3600;
    Debug::SetSettings(settings);

    std::string thresholdMessage = "This message is long enough to hit the 100 byte limit!";
    Debug::Log(thresholdMessage);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string nextMessage = "I am in the new file";
    Debug::Log(nextMessage);

    // 4. Verification
    auto it = fs::directory_iterator("logs/all");
    int fileCount = 0;
    bool foundThresholdInOld = false;
    bool foundNextInNew = false;

    for (const auto& entry : it) {
        fileCount++;
        std::string content = ReadFile(entry.path());
        if (content.find(thresholdMessage) != std::string::npos) {
            foundThresholdInOld = true;
            EXPECT_EQ(content.find(nextMessage), std::string::npos);
        }
        if (content.find(nextMessage) != std::string::npos) {
            foundNextInNew = true;
        }
    }

    EXPECT_EQ(fileCount, 2);
    EXPECT_TRUE(foundThresholdInOld);
    EXPECT_TRUE(foundNextInNew);
}

TEST_F(DebugLogSettingsTest, DeletesOldLogsBasedOnTime) {
    fs::path logDir = "logs/all";

    CreateDummyLog(logDir, -7200, "Old Log");
    CreateDummyLog(logDir, -60, "Fresh Log");

    Debug::Settings settings;
    settings.rootPath = "";
    settings.maxFileSize = 1024 * 1024;
    settings.maxLogFilesAmount = 100;
    settings.deleteLogsAfter = 3600;

    Debug::SetSettings(settings);

    int fileCount = 0;
    bool foundFresh = false;
    bool foundOld = false;
    bool foundCurrent = false;

    for (const auto& entry : fs::directory_iterator(logDir)) {
        fileCount++;
        std::string content = ReadFile(entry.path());
        if (content.find("Old Log") != std::string::npos) foundOld = true;
        if (content.find("Fresh Log") != std::string::npos) foundFresh = true;
    }

    EXPECT_FALSE(foundOld) << "File older than retention period should be deleted";
    EXPECT_TRUE(foundFresh) << "File within retention period should remain";
}

TEST_F(DebugLogSettingsTest, EnforcesMaxLogFilesAmount) {
    fs::path logDir = "logs/all";

    CreateDummyLog(logDir, -500, "File 1");
    CreateDummyLog(logDir, -400, "File 2");
    CreateDummyLog(logDir, -300, "File 3");
    CreateDummyLog(logDir, -200, "File 4");
    CreateDummyLog(logDir, -100, "File 5");

    Debug::Settings settings;
    settings.rootPath = "";
    settings.maxFileSize = 1024 * 1024;
    settings.maxLogFilesAmount = 3;
    settings.deleteLogsAfter = 99999;

    Debug::SetSettings(settings);

    int count = 0;
    std::vector<std::string> fileContents;
    for (const auto& entry : fs::directory_iterator(logDir)) {
        count++;
        fileContents.push_back(ReadFile(entry.path()));
    }

    EXPECT_EQ(count, 3) << "Should only keep 'maxLogFilesAmount' files";

    bool keptOldest = false;
    bool keptNewestDummy = false;

    for(const auto& content : fileContents) {
        if(content.find("File 1") != std::string::npos) keptOldest = true;
        if(content.find("File 5") != std::string::npos) keptNewestDummy = true;
    }

    EXPECT_FALSE(keptOldest) << "Oldest file should have been removed";
    EXPECT_TRUE(keptNewestDummy) << "Newest dummy file should have been kept";
}