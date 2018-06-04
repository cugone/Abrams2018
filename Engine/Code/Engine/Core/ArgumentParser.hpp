#pragma once

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
    ArgumentParser(const std::string& args);
    ~ArgumentParser() = default;
    bool GetNext(Rgba& value);
    bool GetNext(Vector2& value);
    bool GetNext(Vector3& value);
    bool GetNext(Vector4& value);
    bool GetNext(IntVector2& value);
    bool GetNext(IntVector3& value);
    bool GetNext(IntVector4& value);
    bool GetNext(Matrix4& value);
    bool GetNext(std::string& value);
    bool GetNext(unsigned char& value);
    bool GetNext(signed char& value);
    bool GetNext(char& value);
    bool GetNext(unsigned short& value);
    bool GetNext(short& value);
    bool GetNext(unsigned int& value);
    bool GetNext(int& value);
    bool GetNext(unsigned long& value);
    bool GetNext(long& value);
    bool GetNext(unsigned long long& value);
    bool GetNext(long long& value);
    bool GetNext(float& value);
    bool GetNext(double& value);
    bool GetNext(long double& value);

protected:
private:
    std::string _args{};
    std::string _current{};
};