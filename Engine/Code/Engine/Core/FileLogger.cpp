#include "Engine/Core/FileLogger.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

#include <chrono>
#include <filesystem>

FileLogger::~FileLogger() {
    Shutdown();
}

void FileLogger::Log_helper() {
    while(IsRunning()) {
        while(!_queue.empty()) {
            std::scoped_lock<std::mutex> lock(_cs);
            auto str = _queue.front();
            _queue.pop();
            _stream.write(str.data(), str.size());
        }
        RequestFlush();
    }
}

void FileLogger::RequestFlush() {
    if(_requesting_flush) {
        _stream.flush();
        _requesting_flush = false;
    }
}

bool FileLogger::IsRunning() {
    bool running = false;
    {
    std::scoped_lock<std::mutex> lock(_cs);
    running = _is_running;
    }
    return running;
}

void FileLogger::Initialize(const std::string& log_name) {
    namespace FS = std::experimental::filesystem;
    std::string log_str = "Data/Logs/" + log_name + ".log";
    FS::path p{ log_str };
    FileUtils::CreateFolders(p.string());
    _path = p.string();
    _is_running = true;
    _stream.open(_path);
    if(_stream.fail()) {
        DebuggerPrintf("FileLogger failed to initialize.\n");
        _stream.clear();
        _is_running = false;
    }
    _worker = std::thread([this]() { this->Log_helper(); });
    std::string init_log = "Initializing Logger: " + log_name + ".log...";
    LogLine(init_log);
}

void FileLogger::Shutdown() {
    if(IsRunning()) {
        SetIsRunning(false);
        _worker.join();
        _stream.flush();
        _stream.close();
    }
}

void FileLogger::Log(const std::string& msg) {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(msg);
}

void FileLogger::LogLine(const std::string& msg) {
    std::scoped_lock<std::mutex> lock(_cs);
    _queue.push(msg + '\n');
}

void FileLogger::LogAndFlush(const std::string& msg) {
    Log(msg);
    Flush();
}

void FileLogger::LogLineAndFlush(const std::string& msg) {
    LogLine(msg);
    Flush();
}

void FileLogger::Flush() {
    _requesting_flush = true;
    while(_requesting_flush) {
        std::this_thread::yield();
    }
}

void FileLogger::SetIsRunning(bool value /*= true*/) {
    std::scoped_lock<std::mutex> lock(_cs);
    _is_running = value;
}
