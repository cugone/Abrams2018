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
    Audio() = default;
    Audio(const std::string& filename);
    ~Audio();

    BufferType GetType() const;
    const std::vector<unsigned char>& GetBuffer() const;
protected:
private:
    std::vector<unsigned char> _buffer{};
    BufferType _type = Audio::BufferType::None;
};