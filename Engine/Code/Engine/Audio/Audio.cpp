#include "Engine/Audio/Audio.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"

Audio::Audio(const std::string& filename) {
    GUARANTEE_OR_DIE(FileUtils::ReadBufferFromFile(_buffer, filename), "Buffer failed to read from file.");

}

Audio::BufferType Audio::GetType() const {
    return _type;
}

const std::vector<unsigned char>& Audio::GetBuffer() const {
    return _buffer;
}

