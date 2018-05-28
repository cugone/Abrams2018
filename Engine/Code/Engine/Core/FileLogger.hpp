#pragma once

#include "Engine/Core/ThreadSafeQueue.hpp"

#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

class FileLogger {
public:
    FileLogger() = default;
    ~FileLogger();

    void Initialize(const std::string& log_name);
    void Shutdown();
    void Log(const std::string& msg);
    void LogLine(const std::string& msg);
    void LogAndFlush(const std::string& msg);
    void LogLineAndFlush(const std::string& msg);
    void Flush();
    void SetIsRunning(bool value = true);
protected:
private:
    void Log_helper();
    void RequestFlush();
    bool IsRunning();
    std::mutex _cs{};
    std::ofstream _stream{};
    std::string _path{};
    std::thread _worker{};
    ThreadSafeQueue<std::string> _queue;
    bool _is_running = false;
    bool _requesting_flush = false;
};