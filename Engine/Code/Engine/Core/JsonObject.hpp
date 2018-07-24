#pragma once

#include <string>

namespace DataUtils::Json {

class Object {
public:
    Object();
    ~Object();

    std::string key;
    std::string value;

protected:
private:

};

} //End DataUtils::Json