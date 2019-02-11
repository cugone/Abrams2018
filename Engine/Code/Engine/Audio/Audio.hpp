#pragma once

#include <string>
#include <vector>

class Audio {
public:
    enum class BufferType {
        None,
        Riff,
        Ogg,
    };
    Audio(const std::string& filename);

    BufferType GetType() const;
    const std::vector<unsigned char>& GetBuffer() const;
protected:
private:
    std::vector<unsigned char> _buffer{};
    BufferType _type = Audio::BufferType::None;
};