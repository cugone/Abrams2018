#include "Engine/Core/ArgumentParser.hpp"

#include "Engine/Core/Rgba.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

#include <sstream>

ArgumentParser::ArgumentParser(const std::string& args) noexcept
    : _args(args)
    , _current(args)
{
    /* DO NOTHING */
}

bool ArgumentParser::GetNext(Rgba& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = Rgba(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(Vector2& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector2(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(Vector3& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector3(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(Vector4& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = Vector4(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(IntVector2& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector2(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(IntVector3& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector3(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(IntVector4& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = IntVector4(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(Matrix4& value) const noexcept {

    std::string value_str{};
    if(GetNext(value_str)) {
        value = Matrix4(value_str);
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(std::string& value) const noexcept {

    if(_current.empty()) {
        return false;
    }
    std::istringstream ss;
    ss.str(_current);
    std::string arg;
    bool inQuote = false;
    if(ss >> arg) {
        if(arg.front() == '"') {
            inQuote = true;
        }
        if(arg.back() == '"') {
            inQuote = false;
            arg.erase(0, 1);
            arg.pop_back();
        }
        if(inQuote) {
            std::string next;
            while(ss >> next) {
                arg += " ";
                arg += next;
                if(next.back() == '"') {
                    arg.erase(0, 1);
                    arg.pop_back();
                    inQuote = false;
                    break;
                }
            }
        }
        if(!std::getline(ss, _current)) {
            _current = std::string("");
        }
        value = arg;
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(unsigned char& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned char>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(signed char& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<signed char>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(char& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        value = *value_str.begin();
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(unsigned short& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned short>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(short& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<short>(std::stoi(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(unsigned int& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = static_cast<unsigned int>(std::stoul(value_str));
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(int& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoi(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(unsigned long& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoul(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(long& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stol(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(unsigned long long& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoull(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(long long& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stoll(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(float& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stof(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(double& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stod(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}

bool ArgumentParser::GetNext(long double& value) const noexcept {

    std::string value_str;
    if(GetNext(value_str)) {
        try {
            value = std::stold(value_str);
        } catch(std::invalid_argument& /*e*/) {
            return false;
        }
        return true;
    }
    return false;
}
