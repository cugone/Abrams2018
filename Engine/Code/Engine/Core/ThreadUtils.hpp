#pragma once

#include <string>
#include <thread>

namespace ThreadUtils {
    void SetThreadDescription(std::thread& thread, const std::string& description);
    void SetThreadDescription(std::thread& thread, const std::wstring& description);
    void GetThreadDescription(std::thread& thread, std::string& description);
    void GetThreadDescription(std::thread& thread, std::wstring& description);
}
