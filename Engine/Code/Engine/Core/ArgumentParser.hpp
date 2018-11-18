#pragma once

#include <bitset>
#include <string>

class Rgba;
class Vector2;
class Vector3;
class Vector4;
class IntVector2;
class IntVector3;
class IntVector4;
class Matrix4;

class ArgumentParser {
public:
    ArgumentParser(const std::string& args) noexcept;
    ~ArgumentParser() = default;
    template<typename T>
    friend ArgumentParser& operator>>(ArgumentParser& parser, T&& arg);
    operator bool() const;
    bool GetNext(Rgba& value) const noexcept;
    bool GetNext(Vector2& value) const noexcept;
    bool GetNext(Vector3& value) const noexcept;
    bool GetNext(Vector4& value) const noexcept;
    bool GetNext(IntVector2& value) const noexcept;
    bool GetNext(IntVector3& value) const noexcept;
    bool GetNext(IntVector4& value) const noexcept;
    bool GetNext(Matrix4& value) const noexcept;
    bool GetNext(std::string& value) const noexcept;
    bool GetNext(unsigned char& value) const noexcept;
    bool GetNext(signed char& value) const noexcept;
    bool GetNext(char& value) const noexcept;
    bool GetNext(unsigned short& value) const noexcept;
    bool GetNext(short& value) const noexcept;
    bool GetNext(unsigned int& value) const noexcept;
    bool GetNext(int& value) const noexcept;
    bool GetNext(unsigned long& value) const noexcept;
    bool GetNext(long& value) const noexcept;
    bool GetNext(unsigned long long& value) const noexcept;
    bool GetNext(long long& value) const noexcept;
    bool GetNext(float& value) const noexcept;
    bool GetNext(double& value) const noexcept;
    bool GetNext(long double& value) const noexcept;

protected:
private:
    mutable std::string _current{};
    mutable std::bitset<4> _state_bits{};
};

template<typename T>
ArgumentParser& operator>>(ArgumentParser& parser, T&& arg) {
    parser.GetNext(std::forward<T>(arg));
    return parser;
}
