#pragma once

#include "Engine/Core/Win.hpp"

#include <string>

class Clipboard {
public:
    Clipboard() = default;
    Clipboard(HWND hwnd);
    Clipboard(const Clipboard& other) = default;
    Clipboard(Clipboard&& other) = default;
    Clipboard& operator=(const Clipboard& other) = default;
    Clipboard& operator=(Clipboard&& other) = default;
    ~Clipboard();

    bool Open(HWND hwnd);
    bool IsOpen() const;
    bool IsClosed() const;
    static bool HasText();
    static bool HasUnicodeText();
    bool Copy(const std::string& text);
    bool Copy(const std::wstring& text);
    std::string Paste();
    bool Empty();
    void Close();

protected:
private:
    HWND _hwnd{};
    bool _is_open{ false };
};
