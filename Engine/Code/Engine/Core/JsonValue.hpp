#pragma once

#include <string>

namespace DataUtils::Json {

class Value {
public:
    Value();
    ~Value();

    std::string key;
    std::string value;

protected:
private:

};

} //End DataUtils::Json