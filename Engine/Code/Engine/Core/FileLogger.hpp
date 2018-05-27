#pragma once

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

protected:
private:
    void Log_helper();
    std::mutex _cs{};
    std::ofstream _stream{};
    std::ostringstream _stream_buffer{};
    std::string _path{};
    std::thread _worker{};
    bool _is_running = false;
    bool _has_work = false;
    bool _shutting_down = false;
};