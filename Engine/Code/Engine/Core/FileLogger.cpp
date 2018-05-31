#include "Engine/Core/FileLogger.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/TimeUtils.hpp"

#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <filesystem>
#include <iostream>

bool FileLogger::_is_running = false;

FileLogger::~FileLogger() {
    Shutdown();
}

void FileLogger::Log_worker() {
    while(IsRunning()) {
        while(!_queue.empty()) {
            std::scoped_lock<std::mutex> lock(_cs);
            auto str = _queue.front();
            _queue.pop();
            _stream << str;
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
    if(IsRunning()) {
        LogLine("FileLogger already running.");
        return;
    }
    namespace FS = std::experimental::filesystem;
    std::string log_str = "Data/Logs/" + log_name + ".log";
    FS::path p{ log_str };
    FileUtils::CreateFolders(p.string());
    std::string pathAsString = p.string();
    _is_running = true;
    _stream.open(pathAsString);
    if(_stream.fail()) {
        DebuggerPrintf("FileLogger failed to initialize.\n");
        _stream.clear();
        _is_running = false;
        return;
    }
    _old_cout = std::cout.rdbuf(_stream.rdbuf());
    _worker = std::thread([this]() { this->Log_worker(); });
    std::ostringstream ss;
    ss << "Initializing Logger: " << log_str << "...";
    LogLine(ss.str().c_str());
}

void FileLogger::Shutdown() {
    if(IsRunning()) {
        SetIsRunning(false);
        _worker.join();
        _stream.flush();
        _stream.close();
        std::cout.rdbuf(_old_cout);
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

void FileLogger::LogPrint(const std::string& msg) {
    LogTag("log", msg);
}

void FileLogger::LogWarn(const std::string& msg) {
    LogTag("warning", msg);
}

void FileLogger::LogError(const std::string& msg) {
    LogTag("error", msg);
}

void FileLogger::LogTag(const std::string& tag, const std::string& msg) {

    std::stringstream ss;
    InsertTimeStamp(ss);
    InsertTag(ss, tag);
    InsertMessage(ss, msg);

    _queue.push(ss.str());

}

void FileLogger::LogPrintLine(const std::string& msg) {
    LogTagLine("log", msg);
}

void FileLogger::LogWarnLine(const std::string& msg) {
    LogTagLine("warning", msg);
}

void FileLogger::LogErrorLine(const std::string& msg) {
    LogTagLine("error", msg);
}

void FileLogger::LogTagLine(const std::string& tag, const std::string& msg) {
    std::string msgWithLine = msg + '\n';
    LogTag(tag, msgWithLine);
}

void FileLogger::InsertTimeStamp(std::stringstream& msg) {
    msg << "[" << TimeUtils::GetDateTimeStampFromNow(true) << "]";
}

void FileLogger::InsertTag(std::stringstream& msg, const std::string& tag) {
    msg << "[" << tag << "]";
}

void FileLogger::InsertMessage(std::stringstream& msg, const std::string& messageLiteral) {
    msg << messageLiteral;
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
