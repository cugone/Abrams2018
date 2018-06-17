#pragma once

#include "Engine/Core/StringUtils.hpp"

#include <string>
#include <vector>

class Riff {
public:
    static struct ID {
        static constexpr const uint32_t RIFF = StringUtils::FourCC("RIFF");
        static constexpr const uint32_t LIST = StringUtils::FourCC("LIST");
        static constexpr const uint32_t WAVE = StringUtils::FourCC("WAVE");
        static constexpr const uint32_t INFO = StringUtils::FourCC("INFO");
    }ID;

    static constexpr const unsigned int RIFF_SUCCESS = 0;
    static constexpr const unsigned int RIFF_ERROR_NOT_A_RIFF = 1;
    static constexpr const unsigned int RIFF_ERROR_INVALID_RIFF = 2;
    static constexpr const unsigned int RIFF_ERROR_INVALID_ARGUMENT = 3;

    struct RiffHeader {
        char fourcc[4];
        uint32_t length{};
    };
    struct RiffSubChunk {
        char fourcc[4];
        uint8_t* data = nullptr;
        ~RiffSubChunk() {
            delete[] data;
            data = nullptr;
        }
    };
    struct RiffChunk {
        RiffHeader header{};
        RiffSubChunk* data = nullptr;
        ~RiffChunk() {
            delete[] data;
            data = nullptr;
        }
    };
    Riff() = default;
    ~Riff();

    RiffChunk* GetNextChunk() const;
    unsigned int Load(const std::string& filename);
    unsigned int Load(const std::vector<unsigned char>& data);
protected:
private:

    bool IsRecognizableChunk(const char* id) const;
    bool ParseDataIntoChunks(std::vector<unsigned char>& buffer);

    void ShowRiffChunkHeaders();
    std::vector<RiffChunk*> _chunks{};
    mutable std::vector<RiffChunk*>::iterator _current_chunk{};

    friend class Wav;
};