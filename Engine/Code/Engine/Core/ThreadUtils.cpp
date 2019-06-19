#include "Engine/Core/ThreadUtils.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

namespace ThreadUtils {

    void SetThreadDescription(std::thread& thread, const std::string& description) noexcept {
        auto wide_description = StringUtils::ConvertMultiByteToUnicode(description);
        SetThreadDescription(thread, wide_description);
    }

    void SetThreadDescription(std::thread& thread, const std::wstring& description) noexcept {
        ::SetThreadDescription(thread.native_handle(), description.c_str());
    }

    void GetThreadDescription(std::thread& thread, std::string& description) noexcept {
        //TODO: Delegate to GetThreadDescription(std::thread&,std::wstring&)
        PWSTR d{};
        ::GetThreadDescription(thread.native_handle(), &d);
        std::wstring wide_description{d};
        ::LocalFree(d);
        d = nullptr;
        description = StringUtils::ConvertUnicodeToMultiByte(wide_description);
    }

    void GetThreadDescription(std::thread& thread, std::wstring& description) noexcept {
        PWSTR d{};
        ::GetThreadDescription(thread.native_handle(), &d);
        description.assign(d);
        ::LocalFree(d);
        d = nullptr;
    }

}