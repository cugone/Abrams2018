#pragma once

#include <string>
#include <vector>

class Audio {
public:
    enum class BUFFER_TYPE {
        NONE,
        RIFF,
        OGG,
    };
    Audio() = default;
    Audio(const std::string& filename);
    ~Audio();

    BUFFER_TYPE GetType() const;
    const std::vector<unsigned char>& GetBuffer() const;
protected:
private:
    std::vector<unsigned char> _buffer{};
    BUFFER_TYPE _type = Audio::BUFFER_TYPE::NONE;
};