#include "Engine/Core/FileLogger.hpp"

#include "Engine/Core/FileUtils.hpp"

#include <chrono>
#include <filesystem>

FileLogger::~FileLogger() {
    Shutdown();
}

void FileLogger::Log_helper() {
    for(;;) {
        if(_is_running) {
            if(_has_work) {
                std::scoped_lock<std::mutex> lock(_cs);
                _stream.open(_path, std::ios_base::app);
                _stream << _stream_buffer.str();
                _stream.close();
                _stream_buffer.str("");
                _stream_buffer.clear();
                _has_work = false;
            } else {
                std::this_thread::yield();
            }
        } else {
            break;
        }
    }
}


void FileLogger::Initialize(const std::string& log_name) {
    namespace FS = std::experimental::filesystem;
    std::string log_str = "Data/Logs/" + log_name + ".log";
    FS::path p{ log_str };
    FileUtils::CreateFolders(p.string());
    _path = p.string();
    _stream.open(_path);
    _stream.close();
    _is_running = true;
    _worker = std::thread([this](){ this->Log_helper(); });
    _worker.detach();
    std::string init_log = "Initializing Logger: " + log_name + ".log...";
    LogLine(init_log);
}

void FileLogger::Shutdown() {
    std::scoped_lock<std::mutex> lock(_cs);
    if(_is_running) {
        _shutting_down = true;
        _is_running = false;
        if(_worker.joinable()) {
            _worker.join();
        }
    }
}

void FileLogger::Log(const std::string& msg) {
    std::scoped_lock<std::mutex> lock(_cs);
    _stream_buffer << msg;
    _has_work = true;
}

void FileLogger::LogLine(const std::string& msg) {
    std::scoped_lock<std::mutex> lock(_cs);
    _stream_buffer << msg << '\n';
    _has_work = true;
}
