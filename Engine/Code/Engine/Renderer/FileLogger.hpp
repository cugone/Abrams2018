#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <thread>

class FileLogger {
public:
    FileLogger() = default;
    ~FileLogger() = default;

    void Initialize(const std::string& log_name);
    void Shutdown();
    void Log(const std::string& msg);

protected:
private:
    std::mutex _cs{};
    std::ofstream _stream{};
};