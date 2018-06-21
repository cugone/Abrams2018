#pragma once

#include "Engine/Core/ThreadSafeQueue.hpp"

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

class JobSystem;

class FileLogger {
public:
    FileLogger() = default;
    ~FileLogger();

    void Initialize(JobSystem& jobSystem, const std::string& log_name);
    void Shutdown();
    void Log(const std::string& msg);
    void LogLine(const std::string& msg);
    void LogAndFlush(const std::string& msg);
    void LogLineAndFlush(const std::string& msg);
    void LogPrint(const std::string& msg);
    void LogWarn(const std::string& msg);
    void LogError(const std::string& msg);
    void LogTag(const std::string& tag, const std::string& msg);
    void LogPrintLine(const std::string& msg);
    void LogWarnLine(const std::string& msg);
    void LogErrorLine(const std::string& msg);
    void LogTagLine(const std::string& tag, const std::string& msg);
    void Flush();
    void SetIsRunning(bool value = true);
protected:
private:
    void InsertTimeStamp(std::stringstream& msg);
    void InsertTag(std::stringstream& msg, const std::string& tag);
    void InsertMessage(std::stringstream& msg, const std::string& messageLiteral);

    void Log_worker();
    void RequestFlush();
    bool IsRunning();

    std::mutex _cs{};
    std::ofstream _stream{};
    decltype(std::cout.rdbuf()) _old_cout{};
    std::thread _worker{};
    std::condition_variable _signal{};
    ThreadSafeQueue<std::string> _queue;
    JobSystem* _job_system = nullptr;
    static bool _is_running;
    bool _requesting_flush = false;
};